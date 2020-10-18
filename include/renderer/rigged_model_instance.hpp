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

namespace renderer
{
  struct rigged_model_instance : public ecs::component<rigged_model_instance>
  {
    opengl_model model;
    std::optional<animation_data> animation_structures {};
    
    int animation_index{ 0 };
    animation_time t{ 0 };
    bool is_paused{ 0 };

    size_t model_filename_hash{ 0 };

    const aiScene* aiscene{ nullptr };

    animation_time current_animation_duration()
    {
      return animation_structures->clips[animation_index].duration;
    }
  };
}

#endif
