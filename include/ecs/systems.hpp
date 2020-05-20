#ifndef __SYSTEMS_HPP_
#define __SYSTEMS_HPP_

#include <utility>
#include <vector>

#include <ecs/system_base.hpp>

namespace ecs
{

	class systems
	{
	public:
		explicit systems(std::vector<system_base*> systems);

        void initialize(state& state);
		void update(state& state);
		void cleanup(state& state);

	private:
		std::vector<system_base*> _systems;
	};


}
#endif // !__SYSTEMS_HPP_
