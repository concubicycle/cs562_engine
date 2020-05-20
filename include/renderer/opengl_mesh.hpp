#ifndef __OPENGL_MESH_HPP_
#define __OPENGL_MESH_HPP_

#include <cstdint>
#include "glbinding/gl/gl.h"
#include "glbinding/glbinding.h"

namespace renderer
{
	struct opengl_mesh
	{
		gl::GLuint vao{ 0 };
		gl::GLuint vbo{ 0 };
		gl::GLuint ebo{ 0 };
		std::uint32_t index_count{ 0 };
	};
}

#endif