#include <renderer/ik.hpp>
#include <algorithm>
#include <iostream>
#include <util/sign.hpp>

renderer::ik_ccd::ik_ccd(std::string name, skeleton* skeleton, transforms::transform* transform)
  : _skeleton(skeleton)
  , _end_effector(find_end_effector(name))
  , _result_pose(skeleton, skeleton->joints.size())
  , _transform(transform)
{
  for (size_t joint_i = 0; joint_i < _skeleton->joints.size(); ++joint_i)
  {
    auto& joint = _skeleton->joints[joint_i];
    _result_pose.joint_poses[joint_i] = joint_pose(joint.bind_pose);
  }

  _result_pose.compute_global_pose_buffer();

  auto* cursor = _end_effector;
  do
  {
    _pose_chain.emplace_back(cursor);
    cursor = cursor->parent_index ? &(_skeleton->joints[*cursor->parent_index]) : nullptr;

  } while (cursor != nullptr);
    
  fill_link_sequence_ftb(0);
}

Eigen::Vector3f renderer::ik_ccd::get_target() const
{
  return _target;
}

void renderer::ik_ccd::set_target(const Eigen::Vector3f& t)
{
  _target = t;
}

float* renderer::ik_ccd::get_target_data()
{
  return _target.data();
}

renderer::joint* renderer::ik_ccd::find_end_effector(std::string joint_name)
{
  const auto ref = std::find_if(
    _skeleton->joints.begin(),
    _skeleton->joints.end(), [&](const joint& j) { return j.name == joint_name; });
  return &(*ref);
}

void renderer::ik_ccd::fill_link_sequence_ftb(size_t offset, size_t count)
{
  _link_sequence.clear();
  const auto cap = std::min(_pose_chain.size(), offset + count);
  for (auto i = offset + 1; i < cap; ++i)
  {
    _link_sequence.push_back(&(_pose_chain[i]));
  }
}

void renderer::ik_ccd::fill_link_sequence_btf(size_t offset, size_t count)
{
  _link_sequence.clear();
  const auto start = std::max(static_cast<int>(_pose_chain.size() - offset - 1), 0);
  const auto end = std::max(static_cast<int>(_pose_chain.size() - offset - 1 - count), 0);
  for (auto i = start; i > end + 1; --i)
  {
    _link_sequence.push_back(&(_pose_chain[i]));
  }
}

void renderer::ik_ccd::fill_link_sequence_ftb(size_t offset)
{
  fill_link_sequence_ftb(offset, _pose_chain.size());
}

void renderer::ik_ccd::fill_link_sequence_btf(size_t offset)
{
  fill_link_sequence_btf(offset, _pose_chain.size());
}

void renderer::ik_ccd::update(float dt)
{
  if (!_enabled) return;

  _elapsed += dt;
  float t = _elapsed / _duration;

  // bring everything to model space
  const Eigen::Matrix4f to_animation_space = _result_pose.global_bone_space_poses[_end_effector->index];
  const Eigen::Vector4f position_ee = to_animation_space * Eigen::Vector4f(0, 0, 0, 1);
  const Eigen::Vector4f world_target(_target[0], _target[1], _target[2], 1);
  const Eigen::Vector4f position_target = _transform->model_matrix().inverse() * world_target;

  if ((position_ee - position_target).norm() < _threshold)
  {
    return;
  }

  std::uint32_t iteration = 0;
  _step_index = 0;
  _animation_space_target = _transform->model_matrix().inverse() * world_target;
  
  while (step_internal() > _thresholdSq)
  {
    iteration++;

    const bool smart_bounce_iteration = iteration / _link_sequence.size();

    if (_smart_bounce && smart_bounce_iteration > _max_iterations)
      break;

    if (!_smart_bounce && iteration > _max_iterations)
      break;
  }
}

void renderer::ik_ccd::step()
{
  const Eigen::Vector4f world_target(_target[0], _target[1], _target[2], 1);
  _animation_space_target = _transform->model_matrix().inverse() * world_target;
  step_internal();
}


float fast_acos(float x) {
  return (-0.69813170079773212f * x * x - 0.87266462599716477f) * x + 1.5707963267948966f;
}

float renderer::ik_ccd::step_internal()
{
  Eigen::Vector4f position_ee = _result_pose.global_bone_space_poses[_end_effector->index].col(3);

  auto* j = _link_sequence.at(_step_index++)->joint;

  if (_step_index == _link_sequence.size())
    _step_index = 0;

  const auto j_id = j->index;

  const Eigen::Vector4f position_link = _result_pose.global_bone_space_poses[j_id].col(3);
  const Eigen::Vector3f u = (position_ee - position_link).head<3>().normalized();
  const Eigen::Vector3f v = (_animation_space_target - position_link).head<3>().normalized();
  auto dot = u.dot(v);

  if (std::abs(dot) > 1.f)
    dot = util::sign(dot);

  const Eigen::Vector3f world_axis = u.cross(v).normalized();
  Eigen::Vector4f world_axis_h;
  world_axis_h << world_axis, 0;
  const Eigen::Vector3f bone_axis = (
    j->mesh_to_bone_space * _result_pose.global_bone_space_poses[j_id].inverse() * world_axis_h
    ).head<3>();

  const float angle = fast_acos(dot);
  const math::quat<float> my_rotation(bone_axis, angle);
  _result_pose.joint_poses[j_id].rotation = my_rotation * _result_pose.joint_poses[j_id].rotation;  
  _result_pose.compute_global_pose_buffer(j_id);
    
  position_ee = _result_pose.global_bone_space_poses[_end_effector->index].col(3);

  if (_smart_bounce && std::abs(angle) > _smart_bounce_tolerance)
  {
    _step_index = 0;
  }

  return (_animation_space_target - position_ee).squaredNorm();
}

void renderer::ik_ccd::toggle()
{
  _enabled = !_enabled;
  _step_index = 0;
}

void renderer::ik_ccd::reset()
{
  _step_index = 0;
  for (size_t joint_i = 0; joint_i < _skeleton->joints.size(); ++joint_i)
  {
    auto& joint = _skeleton->joints[joint_i];
    _result_pose.joint_poses[joint_i] = joint_pose(joint.bind_pose);
  }
}

renderer::skeleton_pose& renderer::ik_ccd::result_pose()
{
  return _result_pose;
}
