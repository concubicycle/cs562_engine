#ifndef __MATERIAL_TEXTURE_HPP_
#define __MATERIAL_TEXTURE_HPP_

#include <glbinding/gl/gl.h>
#include <optional>

namespace renderer
{
	struct opengl_texture
	{
		opengl_texture(
			gl::GLenum index)
			: texture_index(index)
		{}

		opengl_texture(
			gl::GLenum index,
			std::uint32_t width,
			std::uint32_t height,
			gl::GLenum internal_format = gl::GLenum::GL_RGBA32F,
			gl::GLenum format = gl::GLenum::GL_RGBA)
			: texture_index(index)
		{
			using namespace gl;

			gl::GLuint result;			
			glGenTextures(1, &result);
			texture_id = result;

			glBindTexture(GL_TEXTURE_2D, result);						
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				internal_format,
				width,
				height,
				0,
				format,
				GL_FLOAT,
				nullptr);
			
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}

		gl::GLenum texture_index;
		std::optional<gl::GLuint> texture_id;
	};
}


#endif