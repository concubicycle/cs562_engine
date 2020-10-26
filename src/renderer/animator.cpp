#include <renderer/animator.hpp>
#include <renderer/rigged_model_instance.hpp>
#include <iostream>

renderer::animator::animator(core::frame_timer& timer) : _timer(timer)
{
}

void renderer::animator::update(ecs::state& state)
{
	state.each<rigged_model_instance>([&](rigged_model_instance& component) {
		auto& current_animation = component.current_clip();
		auto timelines = component.get_timelines();
		
		timelines.global.update(_timer.smoothed_delta());
		timelines.local.update_local_time(timelines.global, current_animation.is_looping);

		component.animation_structures->set_pose_buffer_to(component.animation_index(), current_animation.timeline.t);
		component.animation_structures->pose_buffer.compute_global_pose_buffer();
	});
}
