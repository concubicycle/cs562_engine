#ifndef __PUNCTUAL_LIGHT_HPP_
#define __PUNCTUAL_LIGHT_HPP_

#include <Eigen/Core>
#include <ecs/component.hpp>

namespace renderer
{
	struct punctual_light : public ecs::component<punctual_light>
	{
		Eigen::Array3f color;
		float intensity;		
	};
}


#endif