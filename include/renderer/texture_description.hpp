#ifndef __TEXTURE_DESCR_HPP_
#define __TEXTURE_DESCR_HPP_

#include <glbinding/gl/gl.h>

namespace renderer
{
	struct texture_description
	{
		//appease stl
		texture_description() {}

		texture_description(
			gl::GLenum attachment,
			float width, 
			float height,
			gl::GLenum internal_format,
			gl::GLenum format,
			gl::GLenum type)
			: attachment(attachment)
			, height(height)
			, internal_format(internal_format)
			, format(format)
			, width(width)
			, type(type)
		{
		}

		gl::GLenum attachment;
		float width, height;
		gl::GLenum internal_format;
		gl::GLenum format;		
		gl::GLenum type;
		gl::GLuint id{ 0 };
	};
}


#endif