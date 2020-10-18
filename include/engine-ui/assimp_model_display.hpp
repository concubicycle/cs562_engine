#ifndef __ASSIMP_MODEL_DISPLAY_HPP_
#define __ASSIMP_MODEL_DISPLAY_HPP_

#include <assimp/scene.h>
#include <imgui.h>
#include <engine-ui/view.hpp>
#include <ecs/state.hpp>
#include <renderer/skeleton_joint.hpp>

namespace engineui
{
  struct display_node;

  class assimp_model_display : public view
  {
  public:
    assimp_model_display(core::glfw_context& glfw, ecs::state& state);

    void draw() override;

  private:
    ecs::state& _state;

    void print_nodes_recurse(display_node* node, size_t anim_count);
  };
}

#endif