#ifndef __SKELETON_STRUCTS_HPP_
#define __SKELETON_STRUCTS_HPP_

#include <renderer/skeleton_structs.hpp>
#include <spdlog/spdlog.h>
#include <math/interpolate.hpp>

renderer::animation_data::animation_data(const aiScene* scene)
  : joint_count(find_joint_count(scene))
  , pose_buffer(&skeleton, joint_count)
  , bind_pose(&skeleton, joint_count)
{
  skeleton.joints.resize(joint_count);
  pose_buffer.joint_poses.resize(joint_count);
  pose_buffer.global_model_space_poses.resize(joint_count);

  for (auto& m : pose_buffer.global_model_space_poses) m.setIdentity();

  float* mPtr = (float*)(&(scene->mRootNode->mTransformation.a1));
  skeleton.global_inverse = Eigen::Map<Eigen::Matrix4f>(mPtr).transpose().eval().inverse().eval();

  auto flatten_func = [&](
    aiNode* ai_node,
    joint* node,
    joint* parent,
    asset::bone_flattener<joint>& flattener) {
      // assimp is row-major. eigen/opengl are col-major. transpose the matrix
      float* mPtr = (float*)(&(ai_node->mTransformation.a1));
      node->bind_pose = Eigen::Map<Eigen::Matrix4f>(mPtr).transpose().eval();
      node->inverse_bind_pose = node->bind_pose.inverse();

      node->index = flattener.find_node_index(ai_node);
      node->name = std::string(ai_node->mName.data);
      node->mesh_to_bone_space = flattener.find_offset_for_node(ai_node);

      bind_pose.joint_poses[node->index] = joint_pose(node->bind_pose);

      if (parent)
        node->parent_index.emplace(parent->index);
  };

  asset::bone_flattener<joint> flattener(scene, skeleton.joints.data(), joint_count, flatten_func);

  for (size_t animation_index = 0; animation_index < scene->mNumAnimations; ++animation_index)
  {
    auto* animation = scene->mAnimations[animation_index];
    auto duration = animation_time(animation->mDuration * animation->mTicksPerSecond);
    clips.emplace_back(&skeleton, animation, flattener);
  }

  bind_pose.compute_global_pose_buffer();
}

void renderer::animation_data::set_pose_buffer_to(size_t animation_index)
{
  auto& animation = clips.at(animation_index);

  for (size_t joint_idx = 0; joint_idx < skeleton.joints.size(); ++joint_idx)
  {
    auto& joint = skeleton.joints.at(joint_idx);
    auto& joint_clip = animation.joint_clips.at(joint_idx);

    if (joint_clip.is_empty())
      pose_buffer.joint_poses[joint_idx] = joint_pose(joint.bind_pose);
    else
      pose_buffer.joint_poses[joint_idx] = joint_clip.pose_at(animation.timeline.t);
  }
}

void renderer::animation_data::set_pose_buffer_to_blend(
  size_t animation_index_a,
  size_t animation_index_b,
  float blend_factor)
{
  auto& animation_a = clips.at(animation_index_a);
  auto& animation_b = clips.at(animation_index_b);

  for (size_t joint_idx = 0; joint_idx < skeleton.joints.size(); ++joint_idx)
  {
    auto& joint = skeleton.joints.at(joint_idx);
    auto& joint_clip_a = animation_a.joint_clips.at(joint_idx);
    auto& joint_clip_b = animation_b.joint_clips.at(joint_idx);

    auto pose_a = joint_clip_a.is_empty() ? joint_pose(joint.bind_pose) : joint_clip_a.pose_at(animation_a.timeline.t);
    auto pose_b = joint_clip_b.is_empty() ? joint_pose(joint.bind_pose) : joint_clip_b.pose_at(animation_b.timeline.t);

    pose_buffer.joint_poses[joint_idx] = pose_a.interpolate(pose_b, blend_factor);
  }
}

size_t renderer::animation_data::find_joint_count(const aiScene* scene)
{
  size_t joint_count = 0;
  asset::bone_flattener<joint>::count_nodes(scene->mRootNode, joint_count);
  return joint_count;
}





////
renderer::skeleton_animation_clip::skeleton_animation_clip(
  skeleton* skel,
  aiAnimation* animation,
  asset::bone_flattener<joint>& flattener)
  : clip_skeleton(skel)
  , timeline(animation_time(animation->mDuration / animation->mTicksPerSecond))
{
  joint_clips.resize(skel->joints.size());
  name = std::string(animation->mName.data);

  for (size_t bone_index = 0; bone_index < animation->mNumChannels; ++bone_index)
  {
    auto* ai_node_animation = animation->mChannels[bone_index];

    // no choice but to look up node by name    
    std::string node_name(ai_node_animation->mNodeName.data);
    auto& nodes = flattener.find_nodes(node_name);
    if (nodes.size() > 1) spdlog::warn("Duplicate assimp node name within animation.");
    auto* node = nodes[0];

    auto* joint = flattener.find_user_node(node);
    auto joint_index = flattener.find_node_index(node);
    auto& clip = joint_clips[joint_index];

    for (size_t pos_i = 0; pos_i < ai_node_animation->mNumPositionKeys; ++pos_i)
    {
      auto& pos_key = ai_node_animation->mPositionKeys[pos_i];
      clip.translation.keyframes.emplace_back(
        animation_time(pos_key.mTime / animation->mTicksPerSecond),
        Eigen::Vector3f(pos_key.mValue.x, pos_key.mValue.y, pos_key.mValue.z));
    }

    for (size_t rot_i = 0; rot_i < ai_node_animation->mNumRotationKeys; ++rot_i)
    {
      auto& rot_key = ai_node_animation->mRotationKeys[rot_i];
      clip.rotation.keyframes.emplace_back(
        animation_time(rot_key.mTime / animation->mTicksPerSecond),
        math::quat<float>(rot_key.mValue.w, rot_key.mValue.x, rot_key.mValue.y, rot_key.mValue.z));

      clip.rotation.keyframes.back().value.normalize();
    }

    for (size_t scl_i = 0; scl_i < ai_node_animation->mNumScalingKeys; ++scl_i)
    {
      auto& scl_key = ai_node_animation->mScalingKeys[scl_i];
      clip.scale.keyframes.emplace_back(
        animation_time(scl_key.mTime / animation->mTicksPerSecond),
        Eigen::Vector3f(scl_key.mValue.x, scl_key.mValue.y, scl_key.mValue.z));
    }
  }
}








///
renderer::skeleton_pose::skeleton_pose(renderer::skeleton* s, size_t joint_count)
  : skeleton(s)
{
  joint_poses.resize(joint_count);
  global_bone_space_poses.resize(joint_count);
  global_model_space_poses.resize(joint_count);
}

void renderer::skeleton_pose::compute_global_pose_buffer(size_t start_idx)
{  
  const auto& joint = skeleton->joints[start_idx];
  
  global_bone_space_poses[start_idx] = joint.parent_index
    ? global_bone_space_poses[*joint.parent_index] * joint_poses[start_idx].matrix()
    : joint_poses[start_idx].matrix();

  global_model_space_poses[start_idx] = 
    skeleton->global_inverse *
    global_bone_space_poses[start_idx] *
    skeleton->joints[start_idx].mesh_to_bone_space;

  for (size_t i = start_idx+1; i < skeleton->joints.size(); ++i)
  {
    const auto& joint = skeleton->joints[i];
    const auto parent_index = *(joint.parent_index);

    global_bone_space_poses[i] =
      global_bone_space_poses[parent_index] *
      joint_poses[i].matrix();

    global_model_space_poses[i] =
      skeleton->global_inverse *
      global_bone_space_poses[i] *
      skeleton->joints[i].mesh_to_bone_space;
  }
}


///

Eigen::Matrix4f renderer::joint_pose::matrix() const
{
  using namespace Eigen;
  return
    Eigen::Affine3f(Translation3f(translation)).matrix() *
    rotation.matrix() *
    Eigen::Affine3f(Scaling(scale)).matrix();
}

renderer::joint_pose::joint_pose()
  : rotation()
  , translation(0, 0, 0)
  , scale(1, 1, 1)
{
}

renderer::joint_pose::joint_pose(const Eigen::Matrix4f& mat)
{
  Eigen::Affine3f affine;
  affine.matrix() = mat;
  Eigen::Matrix3f rot;
  Eigen::Matrix3f scl;
  affine.computeRotationScaling(&rot, &scl);

  Eigen::Quaternionf eigenq = Eigen::Quaternionf(rot);
  rotation = math::quat<float>(eigenq.w(), eigenq.x(), eigenq.y(), eigenq.z());
  scale = Eigen::Vector3f(scl.coeff(0, 0), scl.coeff(1, 1), scl.coeff(2, 2));
  translation = Eigen::Vector3f(mat.col(3).x(), mat.col(3).y(), mat.col(3).z());
}

renderer::joint_pose::joint_pose(
  math::quat<float> r,
  Eigen::Vector3f t,
  Eigen::Vector3f s)
  : rotation(r)
  , translation(t)
  , scale(s)
{
}

renderer::joint_pose renderer::joint_pose::interpolate(const joint_pose& other, float blend_factor)
{
  return {
    rotation.slerp(other.rotation, blend_factor),
    math::lerp(translation, other.translation, blend_factor),
    math::lerp(scale, other.scale, blend_factor)
  };
}

renderer::joint_pose renderer::operator*(const Eigen::Matrix4f& m, const joint_pose& jp)
{
  Eigen::Matrix4f result = m * jp.matrix();
  return joint_pose(result);
}

#endif
