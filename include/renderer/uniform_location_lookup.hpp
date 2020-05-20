#ifndef __UNIFORM_LOCATION_LOOKUP_HPP_
#define __UNIFORM_LOCATION_LOOKUP_HPP_

#include <unordered_map>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <renderer/program_uniforms.hpp>

namespace renderer
{

	class uniform_location_lookup
	{
	public:
		const program_uniforms& find_uniforms(gl::GLuint program_id);

	private:
		std::unordered_map<gl::GLuint, program_uniforms> _program_id_to_uniforms;

	};

}

#endif