#include <renderer/render_pass.hpp>


renderer::render_pass::render_pass(shader_program& program, render_pass_opengl_state state)
	: _program(program)
	, _pass_ogl_state(state)
{
}

void renderer::render_pass::bind()
{
	_program.bind();
}

void renderer::render_pass::transition_from(render_pass& other)
{
	_pass_ogl_state.transition_from(other._pass_ogl_state);
	other._program.unbind();
	_program.bind();
}

renderer::shader_program& renderer::render_pass::program()
{
	return _program;
}