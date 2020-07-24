#include <engine-ui/participating_medium_display.hpp>
#include <renderer/participating_medium.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::participating_medium_display::participating_medium_display(
	core::glfw_context& glfw,
	ecs::state& state)
	: view(glfw)
	, _state(state)
{
}

void engineui::participating_medium_display::draw()
{
	_state.each<renderer::participating_medium>([&](renderer::participating_medium& pm) {		
		ImGui::Begin("Participating Medium");

		ImGui::SliderFloat("Beta (density)", &pm.beta, 0.f, 0.25f);
		ImGui::InputFloat3("Initial Intensity", pm.initial_intensity.data());
		ImGui::Checkbox("Use Single Scattering", &pm.use_single_scattering);
		ImGui::SliderFloat("Darken Bias", &pm.darken_bias, -0.2f, 0.2f);
		ImGui::SliderFloat("Light Depth Scale", &pm.light_depth_scale, 0.f, 3.f);
		
		ImGui::End();
	});
}