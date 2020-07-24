#ifndef __PUNCTUAL_LIGHT_HPP_
#define __PUNCTUAL_LIGHT_HPP_

#include <cstdint>
#include <Eigen/Core>
#include <glbinding/gl/gl.h>
#include <ecs/component.hpp>
#include <renderer/framebuffer.hpp>
#include <renderer/opengl_texture.hpp>
#include <transforms/transform.hpp>

namespace renderer
{
	struct punctual_light : public ecs::component<punctual_light>
	{
		Eigen::Array3f color;
		float reference_distance;
		std::uint32_t shadow_map_resolution{ 2048 };
		Eigen::Matrix4f light_view;
		Eigen::Matrix4f light_view_back;

		framebuffer<1> shadowmap_framebuffer
		{
			static_cast<float>(shadow_map_resolution * 2),
			static_cast<float>(shadow_map_resolution),
			texture_description(
				gl::GLenum::GL_COLOR_ATTACHMENT0,
				shadow_map_resolution * 2,
				shadow_map_resolution,
				gl::GLenum::GL_RGBA32F,
				gl::GLenum::GL_RGBA,
				gl::GLenum::GL_FLOAT)
		};

		opengl_texture filter_output_texture {
			gl::GLenum::GL_TEXTURE1,
			shadow_map_resolution * 2,
			shadow_map_resolution,
			gl::GLenum::GL_RGBA32F,
			gl::GLenum::GL_RGBA,
			false
		};
	};
}


#endif