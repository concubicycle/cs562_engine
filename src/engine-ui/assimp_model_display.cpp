#include <engine-ui/assimp_model_display.hpp>
#include <renderer/rigged_model_instance.hpp>
#include <renderer/skeleton_joint.hpp>
#include <asset/bone_flattener.hpp>



struct engineui::display_node
{
  renderer::joint* value;
  std::vector<display_node*> children;
};




engineui::assimp_model_display::assimp_model_display(core::glfw_context& glfw, ecs::state& state)
  : view(glfw)
  , _state(state)
{
}



void engineui::assimp_model_display::draw()
{
  _state.each<renderer::rigged_model_instance>([&](renderer::rigged_model_instance& rmi) {
    std::vector<display_node> buffer;
    size_t joint_count = 0;
    asset::bone_flattener<display_node>::count_nodes(rmi.aiscene->mRootNode, joint_count);
    buffer.resize(joint_count);

    display_node* root = nullptr;

    auto flatten_func = [&](
      aiNode* ai_node,
      display_node* node,
      display_node* parent,
      asset::bone_flattener<display_node>& flattener) {
        if (parent)
          parent->children.push_back(node);
        else
          root = node;

        auto index = flattener.find_node_index(ai_node);
        node->value = &(rmi.animation_structures->skeleton.joints[index]);
    };
    asset::bone_flattener<display_node> flattener(rmi.aiscene, buffer.data(), joint_count, flatten_func);

    ImGui::Begin("Model");
    print_nodes_recurse(root, rmi.animation_index);
    ImGui::End();

    static float t = 0;
    t = (float)rmi.t.count();
    ImGui::Begin("Animation");
    ImGui::SliderFloat("time", &t, 0.f, 10.f);
    ImGui::InputInt("index", &rmi.animation_index);
    ImGui::End();
  });  
}

void engineui::assimp_model_display::print_nodes_recurse(display_node* node, size_t animation_index)
{
  ImGui::TreePush();

  renderer::joint* j = node->value;

  if (ImGui::TreeNode(j->name.c_str()))
  {
    ImGui::TreePop();
  }

  for (auto* ptr : node->children) 
    print_nodes_recurse(ptr, animation_index);

  ImGui::TreePop();
}