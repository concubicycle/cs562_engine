#include <engine-ui/assimp_model_display.hpp>
#include <renderer/rigged_model_instance.hpp>
#include <renderer/skeleton_joint.hpp>
#include <asset/bone_flattener.hpp>

#include <util/constants.hpp>

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
    print_nodes_recurse(root);
    ImGui::End();


    ImGui::SliderFloat("Theta", &rmi.target_theta, 0, 2 * util::Pi_f);
    ImGui::SliderFloat("Radius", &rmi.target_r, 0, 10);
    ImGui::SliderFloat("Height", &rmi.target_height, 0, 10);

    if (ImGui::Button("Step"))
    {
      rmi.ik_solver->step();
    }

    if (ImGui::Button("Toggle"))
    {
      rmi.ik_solver->toggle();
    }

    if (ImGui::Button("Reset"))
    {
      rmi.ik_solver->reset();
    }

    if (ImGui::Button("Front to Back"))
    {
      rmi.ik_solver->fill_link_sequence_ftb(0);
    }

    if (ImGui::Button("Back To Front"))
    {
      rmi.ik_solver->fill_link_sequence_btf(0);
    }

    ImGui::Checkbox("Smart Bounce", &rmi.ik_solver->_smart_bounce);

    if (!rmi.has_animations()) return;


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

    ImGui::SliderFloat("global playback rate", &timelines.global.playback_rate, 0, 2);       
    
    const char* current = rmi.current_clip().name.c_str();

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

void engineui::assimp_model_display::print_nodes_recurse(display_node* node)
{
  renderer::joint* j = node->value;

  if (ImGui::TreeNode(j->name.c_str()))
  {
    for (auto* ptr : node->children)
      print_nodes_recurse(ptr);

    ImGui::TreePop();
  }
}