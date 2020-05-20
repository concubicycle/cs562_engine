#include <renderer/uniform_location_lookup.hpp>

const renderer::program_uniforms& renderer::uniform_location_lookup::find_uniforms(gl::GLuint program_id)
{
	return _program_id_to_uniforms.find(program_id)->second;
}
