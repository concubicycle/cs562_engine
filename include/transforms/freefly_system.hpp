#ifndef __FREEFLY_SYSTEM_HPP_
#define __FREEFLY_SYSTEM_HPP_

#include <ecs/system_base.hpp>
#include <core/input_manager.hpp>
#include <core/frame_timer.hpp>

namespace transforms
{
	class freefly_system : public ecs::system_base
	{
	public:
		freefly_system(core::input_manager& input, core::frame_timer& timer);

		virtual void update(ecs::state& state);

	private:
		core::input_manager& _input;
		core::frame_timer& _timer;
	};
}

#endif