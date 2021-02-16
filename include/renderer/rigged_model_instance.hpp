#ifndef __RIGGED_MODEL_INSTANCE_HPP_
#define __RIGGED_MODEL_INSTANCE_HPP_

#include <ecs/component.hpp>
#include <renderer/renderer_component_bits.hpp>
#include <renderer/opengl_model.hpp>
#include <renderer/skeleton_joint.hpp>
#include <renderer/animation_time.hpp>
#include <vector>
#include <optional>
#include <Eigen/Core>

#include <assimp/scene.h>
#include <renderer/skeleton_structs.hpp>
#include <renderer/timeline.hpp>
#include <renderer/ik.hpp>

namespace renderer
{
  struct animation_timelines
  {
    timeline& global;
    local_timeline& local;
  };

  struct rigged_model_instance : public ecs::component<rigged_model_instance>
  {
    opengl_model model;
    std::optional<animation_data> animation_structures {};
    

    bool is_paused{ 0 };
    size_t model_filename_hash{ 0 };
    const aiScene* aiscene{ nullptr };

    animation_time current_transition_duration{ 0 };
    animation_time current_transition_t{ 0 };
    std::optional<size_t> next_index{};

    // temporary bullshit
    std::optional<ik_ccd> ik_solver;
    float target_theta;
    float target_r;
    float target_height;

    [[nodiscard]] Eigen::Vector3f build_target() const
    {
      return Eigen::Vector3f(std::cos(target_theta), 0, std::sin(target_theta)) * target_r + Eigen::Vector3f::UnitY()* target_height;
    }
    /// 

    void set_animation_index(size_t i);
    size_t animation_index() const;
    bool has_animations() const;
    renderer::skeleton_animation_clip& current_clip();    
    animation_time current_clip_time() const;
    animation_time current_clip_duration() const;
    void increment_global_time(animation_time inc);
    animation_timelines get_timelines();


  private:
    std::optional<size_t> _animation_index{ };
  };
}

#endif
