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

namespace renderer
{
  struct joint
  {    
    size_t index;
    std::string name;

    Eigen::Matrix4f bind_pose;
    Eigen::Matrix4f inverse_bind_pose;
    Eigen::Matrix4f assimp_offset;
    
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
  };

  struct skeleton_pose
  {
    skeleton* skeleton;
    std::vector<Eigen::Matrix4f> joint_poses;
    std::vector<Eigen::Matrix4f> global_joint_poses;

    skeleton_pose(renderer::skeleton* s);
    void compute_global_pose_buffer();

  private:
    Eigen::Matrix4f traverse_up(const joint& j) const;
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

      size_t idx = 0;
      while (prop.keyframes.at(idx).time < clip_time && idx < prop.keyframes.size())
        idx++;

      if (idx == 0) return prop.keyframes[idx].value;

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
    animation_time duration;

    // index aligned
    skeleton* clip_skeleton;
    std::vector<joint_animation_clip> joint_clips;

    skeleton_animation_clip(skeleton* skel, aiAnimation* animation, asset::bone_flattener<joint>& flattener);
  };

  struct animation_data
  {
    // clips and skeleton joints are index aligned
    skeleton skeleton;    
    std::vector<skeleton_animation_clip> clips;

    skeleton_pose pose_buffer{ &skeleton };

    animation_data(const aiScene* scene);
    void set_pose_buffer_to(size_t animation_index, animation_time clip_time);    
  };
}

#endif