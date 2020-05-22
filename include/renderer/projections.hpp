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
		Eigen::Vector3f right_top_far)
	{
		float l = left_bottom_near[0];
		float b = left_bottom_near[1];
		float n = left_bottom_near[2];

		float r = right_top_far[0];
		float t = right_top_far[1];
		float f = right_top_far[2];

		Eigen::Matrix4f m;
		m <<	2.f/(r-l),		0,			0,			-1.f*(r+l)/(r-l),
				0,				2.f/(t-b),	0,			-1.f*(t+b)/(t-b),
				0,				0,			2.f/(f-n),	-1.f*(f+n)/(f-n),
				0,				0,			0,			1;

		return m;
	}

	Eigen::Matrix4f perspective(float fov, float aspect_ratio, float near_positive, float far_positive)
	{
		const float a = aspect_ratio;
		const float c = 1.f / std::tan(fov / 2.f);
		const float n = near_positive;
		const float f = far_positive;

		Eigen::Matrix4f m;
		m <<	c/a,		0,			0,				0,
				0,			c,			0,				0,
				0,			0,			-(f+n)/(f-n),	-2.f * f * n / (f - n),
				0,			0,			-1,				0;
		return m;
	}
}


#endif