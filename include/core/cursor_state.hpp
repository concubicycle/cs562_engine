#ifndef __CURSOR_STATE_HPP_
#define __CURSOR_STATE_HPP_


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace core
{
	class cursor_state
	{
	public:
		cursor_state(GLFWwindow* window);

		void enable();
		void disable();

		void push_enable();
		void push_disable();
		void pop();

	private:
		GLFWwindow* _window;
		int _previous_state;
	};
}


#endif