#ifndef __PARTICIPATING_MEDIUM_DISPLAY_HPP_
#define __PARTICIPATING_MEDIUM_DISPLAY_HPP_

#include <core/glfw_context.hpp>
#include <core/frame_timer.hpp>
#include <ecs/state.hpp>
#include <engine-ui/view.hpp>

namespace engineui
{

	class scene_settings_display : public view
	{
	public:
		scene_settings_display(core::glfw_context& glfw, ecs::state& state);

		void draw();

	private:
		ecs::state& _state;
	};

}

#endif