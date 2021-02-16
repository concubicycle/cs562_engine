#pragma once

#include <engine-ui/view.hpp>
#include <ecs/state.hpp>
#include <imgui.h>
#include "curve_walker.hpp"

class walker_display : public engineui::view
{
public:
	walker_display(core::glfw_context& glfw, ecs::state& state) 
		: view(glfw)
		, _state(state) {}
	
	virtual void draw()
	{
		_state.each<curve_walker>([&](curve_walker& cw) {
			ImGui::Begin("Walker");
			ImGui::SliderFloat("speed", &cw.r, 0.f, 10.f);

			if (ImGui::Button("Reset", { 64, 64 }))
			{
				cw.t = 0;
				cw.d = 0;
				cw.is_stopping = false;
			}

			if (ImGui::Button("Stop", { 64, 64 }))
			{
				cw.stop();
			}

			static bool hugetol;
			if (ImGui::Checkbox("Huge arc table tolerance", &hugetol))
			{
				if (hugetol)
				{
					cw.ark_lengths->recalculate_with_tolerance(10000000);
				}
				else
				{
					cw.ark_lengths->recalculate_with_tolerance(0.00001f);
				}
			}

			ImGui::SliderFloat("Rampup Time", &cw.t_rampup, 0, 10);
			ImGui::SliderFloat("Rampdown Time", &cw.t_rampdown, 0, 10);
			ImGui::SliderFloat("s", &cw.s, 0, cw.curve->max_s());
			ImGui::SliderFloat("d", &cw.d, 0, 20);

			ImGui::End();
		});
	};

private:
	ecs::state& _state;
};