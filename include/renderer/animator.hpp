#ifndef __ANIMATOR_HPP_
#define __ANIMATOR_HPP_

#include <core/frame_timer.hpp>
#include <ecs/system_base.hpp>
#include <renderer/rigged_model_instance.hpp>

namespace renderer
{
	class animator : public ecs::system_base
	{
	public:
		explicit animator(core::frame_timer& timer);
		void update(ecs::state& state) override;

		void play_animation(size_t index, animation_time time = animation_time(0));

	private:
		core::frame_timer& _timer;
	};
}

#endif
