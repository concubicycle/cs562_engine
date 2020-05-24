#ifndef __ENGINE_VIEW_HPP_
#define __ENGINE_VIEW_HPP_

#include <core/glfw_context.hpp>
#include <core/viewport.hpp>

namespace engineui
{

	class view
	{
	public:
		view(core::glfw_context& glfw) : _glfw(glfw) {}
		~view() {}
		virtual void draw() = 0;
		virtual void on_show() {}

	protected:
		core::glfw_context& _glfw;
	};

}

#endif