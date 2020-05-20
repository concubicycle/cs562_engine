#include <engine-ui/entities_view.hpp>

#include <cstdio>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glm/gtc/type_ptr.hpp>


engineui::entities_view::entities_view(core::viewport& viewport, event::EventManager& events, ecs::state& r_ecs_state) :
        view(viewport), _events(events), m_r_ecs_state(r_ecs_state), tree(m_r_ecs_state), _current_entity_selected(nullptr)
{
}

void engineui::entities_view::update(ecs::state state) {
    tree.clear();
    m_r_ecs_state.each_id< transforms::transform>([&](auto id, auto& transform)
    {
        tree.insert(id);
    });
}

void engineui::entities_view::draw()
{
    // Draw tree
    ImGui::SetNextWindowSize({ Width, Height }, ImGuiCond_Once);
    ImGui::Begin("Entities");
    ImGui::SetWindowPos({ 10, 50 }, ImGuiCond_Once);

    ImGui::Unindent();

    tree.preorder([&]() {
                      // On down
                      ImGui::Indent();
                  },
                  [&](engineui::transform_tree::node node) {
                      // On node
                      ImGui::Bullet();
                      if (ImGui::Button(std::to_string(node.id).c_str())) {
                          _current_entity_selected = &(m_r_ecs_state.find_entity(node.id));
                      }
                  },
                  [&]() {
                      // On up
                      ImGui::Unindent();
                  });

    ImGui::End();

    // Draw Inspector window
    ImGui::SetNextWindowSize({ InspectorWidth, InspectorHeight }, ImGuiCond_Once);
    ImGui::Begin("Inspector");
    ImGui::SetWindowPos({ 10, Height+60 }, ImGuiCond_Once);

    if (_current_entity_selected == nullptr) {
        ImGui::Text("Select an entity");
    }
    else {
        auto archetype = _current_entity_selected->archetype_id();
        transforms::transform* tr = &_current_entity_selected->get_component<transforms::transform>();

        ImGui::Text("Entity %s", std::to_string(_current_entity_selected->id()).c_str());
        ImGui::Indent();
        ImGui::Text("Archetype id: %llu", archetype);
        ImGui::Text("Transform:");

        if (tr != nullptr) {
            ImGui::Indent();

			const bool pos_changed = ImGui::InputFloat3("pos", glm::value_ptr(tr->position));
			const bool rot_changed = ImGui::InputFloat3("rot", glm::value_ptr(tr->rotation));
			const bool sca_changed = ImGui::InputFloat3("scl", glm::value_ptr(tr->scale));

			const bool any_changed = pos_changed || rot_changed || sca_changed;
			if (any_changed)
			{
				tr->is_matrix_dirty = true;
			}

            ImGui::Unindent();
        }

    }

    ImGui::Unindent();
    ImGui::End();
}
