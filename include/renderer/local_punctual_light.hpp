#ifndef __LOCAL_PUNCTUAL_LIGHT_HPP_
#define __LOCAL_PUNCTUAL_LIGHT_HPP_

#include <Eigen/Core>
#include <ecs/component.hpp>

namespace renderer
{
	struct local_punctual_light : public ecs::component<local_punctual_light>
	{
		Eigen::Array3f color;
		float radius;
	};
}


#endif