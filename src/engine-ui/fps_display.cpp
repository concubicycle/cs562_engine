#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <core/frame_timer.hpp>
#include <engine-ui/fps_display.hpp>



engineui::fps_display::fps_display(core::viewport& viewport, core::frame_timer& timer) : view(viewport), _frame_timer(timer)
{
}

void engineui::fps_display::draw()
{
    ImGui::SetNextWindowPos({ 0, 0 });
    ImGui::Begin("FPS", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration);

    ImVec4 col = _frame_timer.smoothed_delta_secs() > 0.033f 
        ? ImVec4 {1.f, 1.f, 0.f, 1.f}
        : ImVec4 {0.f, 1.f, 0.f, 1.f};

    ImGui::TextColored(col, _frame_timer.frame_info().c_str());
    ImGui::End();
}
