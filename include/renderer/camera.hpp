#ifndef __CAMERA_HPP_
#define __CAMERA_HPP_

#include <ecs/component.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace renderer
{
	struct camera : public ecs::component<camera>
	{
		Eigen::Matrix4f projection;
		Eigen::Affine3f inverse_view;
		Eigen::Affine3f view;		

		float fov;
		float near_distance;
		float far_distance;
	};
}

#endif
