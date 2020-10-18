#include <renderer/animator.hpp>
#include <renderer/rigged_model_instance.hpp>
#include <iostream>

renderer::animator::animator(core::frame_timer& timer) : _timer(timer)
{
}

void renderer::animator::update(ecs::state& state)
{
	state.each<rigged_model_instance>([&](rigged_model_instance& component) {
		auto anim_duration = component.current_animation_duration();

		component.t += _timer.smoothed_delta();

		if (component.t > anim_duration)
			component.t = component.t - anim_duration;

		component.animation_structures->set_pose_buffer_to(component.animation_index, component.t);
		component.animation_structures->pose_buffer.compute_global_pose_buffer();
	});
}
