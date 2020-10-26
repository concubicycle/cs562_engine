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

    void set_animation_index(size_t i);
    size_t animation_index() const;
    renderer::skeleton_animation_clip& current_clip();    
    animation_time current_clip_time() const;
    animation_time current_clip_duration() const;
    void increment_global_time(animation_time inc);
    animation_timelines get_timelines();


  private:
    size_t _animation_index{ 0 };
  };
}

#endif
