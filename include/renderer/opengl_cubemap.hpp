#ifndef __OPENGL_CUBEMAP_HPP_
#define __OPENGL_CUBEMAP_HPP_

#include <glbinding/gl/gl.h>
#include <renderer/opengl_texture.hpp>

namespace renderer
{
	struct opengl_cubemap
	{
		opengl_texture cubemap{ gl::GL_TEXTURE0 };
		gl::GLuint vao;
		gl::GLuint vbo;
	};
}

#endif
