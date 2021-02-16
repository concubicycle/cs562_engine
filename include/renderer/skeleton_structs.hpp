#ifndef __SKEL_STRUCTS_HPP_
#define __SKEL_STRUCTS_HPP_

#include <iostream>
#include <vector>
#include <optional>
#include <Eigen/Core>
#include <math/quat.hpp>
#include <assimp/scene.h>
#include <stdint.h> 

#include <asset/bone_flattener.hpp>
#include <renderer/animation_time.hpp>
#include <renderer/skeletal_animation_frame.hpp>

#include <renderer/timeline.hpp>

namespace renderer
{
  struct joint
  {    
    size_t index;
    std::string name;

    Eigen::Matrix4f bind_pose;
    Eigen::Matrix4f inverse_bind_pose;
    Eigen::Matrix4f mesh_to_bone_space;
    
    std::optional<size_t> parent_index;
  };

  struct skeleton
  {
    std::vector<joint> joints;
    Eigen::Matrix4f global_inverse;
  };

  struct joint_pose
  {
    math::quat<float> rotation;
    Eigen::Vector3f translation;
    Eigen::Vector3f scale;

    Eigen::Matrix4f matrix() const;

    joint_pose();
    explicit joint_pose(const Eigen::Matrix4f& mat);
    joint_pose(math::quat<float> r, Eigen::Vector3f t, Eigen::Vector3f s);

    joint_pose interpolate(const joint_pose& other, float blend_factor);
  };

  joint_pose operator* (const Eigen::Matrix4f& m, const joint_pose& jp);

  class skeleton_pose
  {
  public:
    skeleton* skeleton;
    std::vector<joint_pose> joint_poses;
    std::vector<Eigen::Matrix4f> global_model_space_poses;
    std::vector<Eigen::Matrix4f> global_bone_space_poses;

    skeleton_pose(renderer::skeleton* s, size_t node_count);    
    void compute_global_pose_buffer(size_t start_idx = 0);
  };


  struct joint_animation_clip
  {
    property_clip<math::quat<float>> rotation{ math::quat<float>::identity };
    property_clip<Eigen::Vector3f> translation{ Eigen::Vector3f::Zero() };
    property_clip<Eigen::Vector3f> scale{ Eigen::Vector3f::Ones() };

    joint_pose pose_at(animation_time time)
    {      
      return {
        anim_lerp(rotation, time).normalized(),
        anim_lerp(translation, time),
        anim_lerp(scale, time)
      };
    }

    bool is_empty()
    {
      return rotation.keyframes.empty() &&
        translation.keyframes.empty() &&
        scale.keyframes.empty();
    }

  private:
    template <typename T>
    T anim_lerp(const property_clip<T>& prop, renderer::animation_time clip_time)
    {
      if (prop.keyframes.empty())
        return prop.identity;
            
      if (clip_time > prop.end_time())
        return prop.end_value();

      if (prop.keyframes.size() == 1)
        return prop.keyframes.front().value;

      size_t idx = 1;
      while (prop.keyframes.at(idx).time < clip_time && idx < prop.keyframes.size())
        idx++;

      auto& key0 = prop.keyframes[idx - 1];
      auto& key1 = prop.keyframes[idx];

      auto blend_factor = (clip_time - key0.time) / (key1.time - key0.time);
      return interpolate(key0.value, key1.value, blend_factor);
    }

    template <typename T>
    T interpolate(const T& a, const T& b, float blend_factor)
    {
      return a + blend_factor * (b - a);
    }

    template <>
    math::quat<float> interpolate(const math::quat<float>& a, const math::quat<float>& b, float blend_factor)
    {
      return a.slerp(b, blend_factor);
    }
  };

  struct skeleton_animation_clip
  {
    std::string name;

    // index aligned
    skeleton* clip_skeleton;
    std::vector<joint_animation_clip> joint_clips;

    local_timeline timeline;

    bool is_looping{ true };

    skeleton_animation_clip(
      skeleton* skel,
      aiAnimation* animation,
      asset::bone_flattener<joint>& flattener);

  };

  struct animation_data
  {
    // index aligned
    skeleton skeleton{};
    std::vector<skeleton_animation_clip> clips{};
    timeline global_timeline{};

    size_t joint_count;
    skeleton_pose pose_buffer;

    skeleton_pose bind_pose;

    animation_data(const aiScene* scene);
    void set_pose_buffer_to(size_t animation_index);
    void set_pose_buffer_to_blend(
      size_t animation_index_a,
      size_t animation_index_b,
      float blend_factor);

  private:
    size_t find_joint_count(const aiScene* scene);

  };
}

#endif
