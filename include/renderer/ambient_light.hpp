#ifndef __AMBIENT_LIGHT_HPP_
#define __AMBIENT_LIGHT_HPP_

#include <ecs/component.hpp>
#include <Eigen/Core>
#include <Eigen/Geometry>

namespace renderer
{
	struct ambient_light : public ecs::component<ambient_light>
	{		
		Eigen::Array3f color;
	};
}

#endif
