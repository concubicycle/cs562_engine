#ifndef __IMGUI_OVERLAY_HPP_
#define __IMGUI_OVERLAY_HPP_

#include <initializer_list>

#include <core/game_input_manager.hpp>
#include <engine-ui/view.hpp>

#include <core/cursor_state.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


namespace engineui
{

	class imgui_overlay
	{
		const char* glsl_version = "#version 420";


	public:
		imgui_overlay(GLFWwindow* window, core::game_input_manager& input, core::cursor_state& cursor);

		void update();

		template<typename... T>
		void register_views(T*... views)
		{
			_views.insert(_views.end(), { views... });
		}

		

	private:
		bool _visible{ false };		
		core::game_input_manager& _input;
		core::cursor_state& _cursor;
		std::vector<view*> _views;
		

		void render_views();



		//
		float some_float{ 1.f };
		char some_string[1024]{ "foo" };
	};

}


#endif