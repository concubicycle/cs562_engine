#ifndef __SKELETON_STRUCTS_HPP_
#define __SKELETON_STRUCTS_HPP_

#include <renderer/skeleton_structs.hpp>
#include <spdlog/spdlog.h>

renderer::animation_data::animation_data(const aiScene* scene)
{
  size_t joint_count = 0;
  asset::bone_flattener<joint>::count_nodes(scene->mRootNode, joint_count);  
  skeleton.joints.resize(joint_count);
  pose_buffer.local_poses.resize(joint_count);
  pose_buffer.global_pose_buffer.resize(joint_count);  
  for (auto& m : pose_buffer.global_pose_buffer) m.setIdentity();

  auto flatten_func = [&](
    aiNode* ai_node,
    joint* node,
    joint* parent,
    asset::bone_flattener<joint>& flattener) {
      float* mPtr = (float*)(&(ai_node->mTransformation.a1));
      // assimp is row-major. eigen/opengl are col-major. transpose the matrix
      node->bind_pose = Eigen::Map<Eigen::Matrix4f>(mPtr).transpose().matrix();
      node->inverse_bind_pose = node->bind_pose.inverse();
      node->index = flattener.find_node_index(ai_node);
      node->name = std::string(ai_node->mName.data);
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
}

void renderer::animation_data::set_pose_buffer_to(size_t animation_index, animation_time clip_time)
{
  auto& animation = clips.at(animation_index);

  for (size_t joint_idx = 0; joint_idx < skeleton.joints.size(); ++joint_idx)
  {
    auto& joint = skeleton.joints.at(joint_idx);
    auto& joint_clip = animation.joint_clips.at(joint_idx);
    pose_buffer.local_poses[joint_idx] = joint_clip.pose_at(clip_time);
  }
}


////
renderer::skeleton_animation_clip::skeleton_animation_clip(
  skeleton* skel, 
  aiAnimation* animation, 
  asset::bone_flattener<joint>& flattener)
  : clip_skeleton(skel)
{
  joint_clips.resize(skel->joints.size());

  name = std::string(animation->mName.data);
  duration = animation_time(animation->mDuration / animation->mTicksPerSecond);

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
renderer::skeleton_pose::skeleton_pose(renderer::skeleton* s) : skeleton(s)
{
  local_poses.resize(skeleton->joints.size());
}

void renderer::skeleton_pose::compute_global_pose_buffer()
{
  // assumes children are stored after parents, so calc parents first
  for (size_t i = 0; i < skeleton->joints.size(); ++i)
  {
    auto& joint = skeleton->joints[i];
    
    if (joint.parent_index)
    {
      global_pose_buffer[i] = global_pose_buffer[*joint.parent_index] * joint.bind_pose; //global_pose_buffer[*joint.parent_index] * local_poses[i].matrix();
    }
  }
}


///
Eigen::Matrix4f renderer::joint_pose::matrix() const
{
  using namespace Eigen;
  Eigen::Matrix4f t = Eigen::Affine3f(Translation3f(translation)).matrix();
  Eigen::Matrix4f r = rotation.matrix();
  Eigen::Matrix4f s = Eigen::Affine3f(Scaling(scale)).matrix();
  return t * r * s;
}
#endif
