#ifndef __PROJECTIONS_HPP_
#define __PROJECTIONS_HPP_

#include <Eigen/Core>

/**
	Math from Real Time Rendering, p94/p99
*/
namespace renderer
{
	Eigen::Matrix4f orthographic(
		Eigen::Vector3f left_bottom_near,
		Eigen::Vector3f right_top_far);

	Eigen::Matrix4f perspective(
		float fov,
		float aspect_ratio,
		float near_positive,
		float far_positive);
}


#endif