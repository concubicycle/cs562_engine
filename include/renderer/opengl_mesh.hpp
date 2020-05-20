#ifndef __OPENGL_MESH_HPP_
#define __OPENGL_MESH_HPP_

#include <cstddef>
#include <cstdint>
#include "glbinding/gl/gl.h"
#include "glbinding/glbinding.h"

#include <renderer/opengl_material.hpp>

namespace renderer
{
	struct opengl_mesh
	{
		gl::GLuint vao{ 0 };
		gl::GLuint vbo{ 0 };
		gl::GLuint ebo{ 0 };
		std::uint32_t index_count{ 0 };

		opengl_material material;
		size_t material_file_hash{ 0 };
	};
}

#endif