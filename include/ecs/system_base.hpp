#ifndef __SYSTEM_BASE_HPP_
#define __SYSTEM_BASE_HPP_

#include <ecs/state.hpp>

namespace ecs
{

	class system_base
	{
	public:
    virtual ~system_base() = default;
    virtual void initialize(ecs::state& state) {};
		virtual void cleanup(ecs::state& state) {};
		virtual void update(ecs::state& state) = 0;
	};

}

#endif // !__SYSTEM_BASE_HPP_
