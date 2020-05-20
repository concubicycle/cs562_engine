#include <renderer/program_uniforms.hpp>

gl::GLuint renderer::program_uniforms::find_location(size_t name_hash) const
{
	return _name_hash_to_location.find(name_hash)->second;
}