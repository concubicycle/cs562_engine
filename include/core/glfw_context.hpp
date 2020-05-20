#ifndef __GLFW_CONTEXT_HPP_
#define __GLFW_CONTEXT_HPP_

#include <iostream>
#include <cstdint>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <core/startup_config.hpp>

namespace core
{

	class glfw_context
	{
	public:
		glfw_context(startup_config& conf);
		~glfw_context();

		GLFWwindow* window();
		const GLFWwindow* window() const;
		void swap_buffers();

		std::uint32_t width() const;
		std::uint32_t height() const;

		void set_should_close(bool);
		void set_minimize_callback(GLFWwindowiconifyfun);
		bool is_minimized();
		void set_fullscreen(bool);
		bool is_fullscreen();
		void set_window_changed_callback(std::function<void()>);

	private:
		GLFWwindow* _window;
		GLFWmonitor* _monitor;
		const GLFWvidmode* _mode;
		startup_config& _conf;

		std::uint32_t _width;
		std::uint32_t _height;
	};

}
#endif