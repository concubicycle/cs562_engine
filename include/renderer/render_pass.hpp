#ifndef __RENDERER_PASS_HPP_
#define __RENDERER_PASS_HPP_

#include <algorithm>
#include <cstdint>
#include <array>
#include <glbinding/gl/gl.h>
#include <renderer/shader_program.hpp>
#include <renderer/render_pass_opengl_state.hpp>

namespace renderer
{
	
	class render_pass
	{
	public:
		render_pass(shader_program& program, render_pass_opengl_state state);

		void bind();		
		void transition_from(render_pass& other);
		shader_program& program();

	private:
		shader_program& _program;
		render_pass_opengl_state _pass_ogl_state;
	};
}

#endif