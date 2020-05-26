#ifndef __TEXTURE_DESCR_HPP_
#define __TEXTURE_DESCR_HPP_

#include <glbinding/gl/gl.h>

namespace renderer
{
	class texture_description
	{
	public:
		// default constructor necessary for storing these in an std::array,
		// without needing to initialize it in initializer list
		texture_description() {}

		texture_description(
			gl::GLenum attachment_in,
			float width,
			float height,
			gl::GLenum internal_format,
			gl::GLenum format,
			gl::GLenum type)			
			: _attachment(attachment_in)
			, _width(width)
			, _height(height)
			, _internal_format(internal_format)
			, _format(format)
			, _type(type)
		{			
			gl::glGenTextures(1, &_id);
		}

		texture_description(const texture_description& other)
			: _id(other._id)
			, _attachment(other._attachment)
			, _width(other._width)
			, _height(other._height)
			, _internal_format(other._internal_format)
			, _format(other._format)
			, _type(other._type) {}

		void operator=(const texture_description& other)
		{
			_id = other._id;
			_attachment = other._attachment;
			_width = other._width;
			_height = other._height;
			_internal_format = other._internal_format;
			_format = other._format;
			_type = other._type;
		}

		gl::GLuint id() { return _id; }
		gl::GLenum attachment(){ return _attachment; }

		void create_attachment()
		{
			using namespace gl;

			glBindTexture(GL_TEXTURE_2D, _id);
			glTexImage2D(GL_TEXTURE_2D, 0, _internal_format, _width, _height, 0, _format, _type, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, GL_TEXTURE_2D, _id, 0);
		}

	private:
		gl::GLuint _id;
		float _width, _height;
		gl::GLenum _internal_format;
		gl::GLenum _format;
		gl::GLenum _attachment;
		gl::GLenum _type;
	};
}


#endif