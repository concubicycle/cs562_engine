#ifndef __MATERIAL_TEXTURE_HPP_
#define __MATERIAL_TEXTURE_HPP_

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <optional>

namespace renderer
{
	struct opengl_texture
	{
		opengl_texture(gl::GLenum index)
			: texture_index(index)
		{}

		gl::GLenum texture_index;
		std::optional<gl::GLuint> texture_id;
	};
}


#endif