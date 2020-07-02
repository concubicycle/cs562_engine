#ifndef __CAMERA_HPP_
#define __CAMERA_HPP_

#include <ecs/component.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

#include <optional>
#include <renderer/opengl_cubemap.hpp>

namespace renderer
{
	enum class background_type
	{
		none = 0, 
		color = 1, 
		cubemap = 2,
		skydome = 3
	};

	struct camera : public ecs::component<camera>
	{
		Eigen::Matrix4f projection;
		Eigen::Affine3f inverse_view;
		Eigen::Affine3f view;
		background_type background;

		std::optional<opengl_cubemap> skybox_cubemap;
		std::optional<gl::GLuint> skydome_texture;
		std::optional<gl::GLuint> skydome_irradiance_map_texture;
		Eigen::Array2f skydome_size;
		Eigen::Array4f clear_color;

		float fov;
		float near_distance;
		float far_distance;
	};
}

#endif
