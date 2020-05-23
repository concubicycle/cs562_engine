#ifndef __FREEFLY_COMPONENT_HPP_
#define __FREEFLY_COMPONENT_HPP_

#include <ecs/component.hpp>

namespace transforms
{
	struct freefly_component : public ecs::component<freefly_component>
	{
		float walk_speed{ 5.f };
	};
}


#endif