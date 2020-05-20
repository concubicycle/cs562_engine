#ifndef __WORLD_HPP_
#define __WORLD_HPP_

#include <ecs/systems.hpp>
#include <ecs/state.hpp>

namespace ecs
{

	class world
	{
	public:
		world(systems& systems, state& state);

		void update()
		{
			_systems.update(_state);
		}

		void initialize()
        {
            _systems.initialize(_state);
        }

		void cleanup()
		{
			_systems.cleanup(_state);
		}

	private:
		systems& _systems;
		state& _state;
	};

}

#endif // !__WORLD_HPP_
