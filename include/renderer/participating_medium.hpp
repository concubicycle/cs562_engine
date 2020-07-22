#ifndef __PARTICIPATING_MEDIUM_HPP_
#define __PARTICIPATING_MEDIUM_HPP_

#include <ecs/component.hpp>
#include <cstdint>
#include <Eigen/Core>

namespace renderer
{
	struct participating_medium : public ecs::component<participating_medium>
	{
		float beta{ 0.04f };
		float f_lookup_range{ 10.f };
		Eigen::Vector3f initial_intensity{ 50000.f, 50000.f, 50000.f };
		bool use_single_scattering{ false };
	};
}


#endif