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

			size_t attachment_index = 0;
			(( init_attachment(vals, attachment_index++) ), ...);

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

		const gl::GLuint &texture(size_t i) const
		{
			return _textures[i].id;
		}

		void bind_textures()
		{

		}


	private:
		gl::GLuint _id;
		gl::GLuint _rbo_depth;
		std::array<texture_description, NumTextures> _textures;
		std::array<gl::GLenum, NumTextures> _attachments;
		

		void init_attachment(texture_description& desc, size_t i)
		{
			_attachments[i] = desc.attachment;
			_textures[i] = desc;
			create_attachment(_textures[i]);
		}

		void create_attachment(texture_description& desc)
		{
			using namespace gl;
						
			glGenTextures(1, &desc.id);
			glBindTexture(GL_TEXTURE_2D, desc.id);
			glTexImage2D(GL_TEXTURE_2D, 0, desc.internal_format, desc.width, desc.height, 0, desc.format, desc.type, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glFramebufferTexture2D(GL_FRAMEBUFFER, desc.attachment, GL_TEXTURE_2D, desc.id, 0);
		}
	};
}


#endif