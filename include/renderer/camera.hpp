#ifndef __CAMERA_HPP_
#define __CAMERA_HPP_

#include <ecs/component.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace renderer
{
	enum class background_type
	{
		none = 0, 
		color = 1, 
		cubemap = 2
	};

	struct camera : public ecs::component<camera>
	{
		Eigen::Matrix4f projection;
		Eigen::Affine3f inverse_view;
		Eigen::Affine3f view;
		background_type background;

		Eigen::Array4f clear_color;

		float fov;
		float near_distance;
		float far_distance;
	};
}

#endif
