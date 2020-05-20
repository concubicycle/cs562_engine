#ifndef __MATERIAL_TEXTURE_HPP_
#define __MATERIAL_TEXTURE_HPP_

#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>


namespace renderer
{
	struct material_texture
	{
		material_texture(const gl::GLenum index)
			: texture_index(index)
		{}

		gl::GLenum texture_index;
		gl::GLuint texture_id;
	};
}


#endif