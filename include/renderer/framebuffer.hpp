#ifndef __FRAMEBUFFER_HPP_
#define __FRAMEBUFFER_HPP_

#include <vector>
#include <array>
#include <glbinding/gl/gl.h>
#include <renderer/texture_description.hpp>

namespace renderer
{
	// reference used: https://learnopengl.com/Advanced-Lighting/Deferred-Shading
	template<size_t NumTextures>
	class framebuffer
	{
	public:

		template <typename ... Ts>
		framebuffer(float width, float height, Ts ... vals)
		{
			using namespace gl;

			glGenFramebuffers(1, &_id);
			glBindFramebuffer(GL_FRAMEBUFFER, _id);

			size_t i = 0; // starting index, can be an argument
			(( init_attachment(vals, i++) ), ...);

			glDrawBuffers(NumTextures, _attachments.data());

			// create and attach depth buffer (renderbuffer)			
			glGenRenderbuffers(1, &_rbo_depth);
			glBindRenderbuffer(GL_RENDERBUFFER, _rbo_depth);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _rbo_depth);
			
			// finally check if framebuffer is complete
			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				throw std::runtime_error("Framebuffer not complete!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		void bind() { gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, _id); }
		void unbind() { gl::glBindFramebuffer(gl::GL_FRAMEBUFFER, 0); }

		std::array<texture_description, NumTextures>& textures()
		{
			return _textures;
		}

	private:
		gl::GLuint _id;
		gl::GLuint _rbo_depth;
		std::array<texture_description, NumTextures> _textures;
		std::array<gl::GLenum, NumTextures> _attachments;
		

		void init_attachment(texture_description& desc, size_t i)
		{
			_attachments[i] = desc.attachment();
			_textures[i] = desc;
			_textures[i].create_attachment();			
		}
	};
}


#endif