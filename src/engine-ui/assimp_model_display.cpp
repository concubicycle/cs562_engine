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
    print_nodes_recurse(root, rmi.animation_index());
    ImGui::End();

    float duration = (float)rmi.current_clip_duration().count();
    auto timelines = rmi.get_timelines();
    auto& clips = rmi.animation_structures->clips;
    
    static float t = 0;
    t = (float)rmi.current_clip_time().count();

    ImGui::Begin("Animator");
    if (ImGui::SliderFloat("time", &t, 0.f, duration))
    {
      auto diff = t - rmi.current_clip_time().count();
      rmi.increment_global_time(renderer::animation_time(diff));
    }

    ImGui::SliderFloat("global playback rate", &rmi.animation_structures->global_timeline.playback_rate, 0, 2);       
    
    const char* current = clips[rmi.animation_index()].name.c_str();

    if (ImGui::BeginCombo("Animations", current))
    {
      for (size_t i = 0; i < clips.size(); ++i)
      {
        const bool is_selected = (current == clips[i].name.c_str());
        if (ImGui::Selectable(clips[i].name.c_str(), is_selected))
          rmi.set_animation_index(i);

        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
        if (is_selected)
          ImGui::SetItemDefaultFocus();

      }
      ImGui::EndCombo();
    }

    ImGui::End();
    });
}

void engineui::assimp_model_display::print_nodes_recurse(display_node* node, size_t animation_index)
{
  renderer::joint* j = node->value;

  if (ImGui::TreeNode(j->name.c_str()))
  {
    for (auto* ptr : node->children)
      print_nodes_recurse(ptr, animation_index);

    ImGui::TreePop();
  }
}