#ifndef __PROGRAM_UNIOFORMS_HPP_
#define __PROGRAM_UNIOFORMS_HPP_

#include <stdlib.h>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

namespace renderer
{
	struct program_uniforms
	{
		gl::GLuint find_location(size_t name_hash) const;

		std::unordered_map<size_t, gl::GLuint> _name_hash_to_location;
	};
}



#endif