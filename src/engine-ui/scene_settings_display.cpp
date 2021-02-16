#include <engine-ui/scene_settings_display.hpp>
#include <renderer/scene_settings.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


engineui::scene_settings_display::scene_settings_display(
	core::glfw_context& glfw,
	ecs::state& state)
	: view(glfw)
	, _state(state)
{
}

void engineui::scene_settings_display::draw()
{
	_state.each<renderer::scene_settings>([&](renderer::scene_settings& settings) {
		ImGui::Begin("Global Graphical Settings");

		if (ImGui::TreeNode("Participating Medium"))
		{
			ImGui::SliderFloat("Beta (density)", &settings.participating_medium.beta, 0.f, 0.25f);
			ImGui::InputFloat3("Initial Intensity", settings.participating_medium.initial_intensity.data());
			ImGui::Checkbox("Use Single Scattering", &settings.participating_medium.use_single_scattering);
			ImGui::SliderFloat("Light Depth Scale", &settings.participating_medium.light_depth_scale, 0.f, 3.f);
			ImGui::TreePop();
		}
				
		if (ImGui::TreeNode("Ambient Occlusion"))
		{
			ImGui::SliderFloat("Range Of Influence", &settings.ambient_occlusion.range_of_influence, 0.f, 15.f);
			ImGui::SliderFloat("C Coefficient", &settings.ambient_occlusion.c_coefficient, 0.f, 2.f);
			ImGui::SliderFloat("Sigma", &settings.ambient_occlusion.sigma, 0.001f, 0.1f);
			ImGui::SliderFloat("Scale", &settings.ambient_occlusion.scale, 0.f, 20.f);
			ImGui::SliderFloat("Contrast", &settings.ambient_occlusion.contrast, 0.f, 20.f);
			static int n_intermediate = settings.ambient_occlusion.n;
			ImGui::InputInt("n", &n_intermediate);
			settings.ambient_occlusion.n = n_intermediate;
			ImGui::Checkbox("Draw AO map", &settings.ambient_occlusion.draw_only_ao);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Settings"))
		{			
			ImGui::Checkbox("Draw Bones", &settings.animation.draw_bones);
			ImGui::SliderFloat("Bone Scale", &settings.animation.bone_scale, 0.01f, 3.f);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Light"))
		{
			ImGui::Checkbox("IBL", &settings.light.ibl);
			ImGui::TreePop();
		}

		ImGui::End();
	});
}