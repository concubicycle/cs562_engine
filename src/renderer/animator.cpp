#include <renderer/animator.hpp>
#include <renderer/rigged_model_instance.hpp>
#include <iostream>

renderer::animator::animator(core::frame_timer& timer) : _timer(timer)
{
}

void renderer::animator::update(ecs::state& state)
{
  state.each<rigged_model_instance>([&](rigged_model_instance& component) {

    if (!component.has_animations()) return;

    auto& current_animation = component.animation_structures->clips[component.animation_index()];
    auto timelines = component.get_timelines();
    timelines.global.update(_timer.smoothed_delta());
    timelines.local.update_local_time(timelines.global, current_animation.is_looping);

    if (component.next_index)
    {
      component.current_transition_t += _timer.smoothed_delta();

      auto& next_animation = component.animation_structures->clips[*component.next_index];
      auto& next_timeline = next_animation.timeline;
      next_timeline.update_local_time(timelines.global, next_animation.is_looping);

      float blend_factor = component.current_transition_t / component.current_transition_duration;
      component.animation_structures->set_pose_buffer_to_blend(component.animation_index(), *component.next_index, blend_factor);

      if (component.current_transition_t >= component.current_transition_duration)
      {
        component.set_animation_index(*component.next_index);
        component.next_index.reset();
      }

      component.animation_structures->pose_buffer.compute_global_pose_buffer();
    }
    else
    {
      component.animation_structures->set_pose_buffer_to(component.animation_index());
      component.animation_structures->pose_buffer.compute_global_pose_buffer();
    }
    });
}
