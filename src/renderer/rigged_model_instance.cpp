#include <renderer/rigged_model_instance.hpp>
#include <renderer/renderer_component_bits.hpp>


template<> const component_shift ecs::component<renderer::rigged_model_instance>::component_bitshift{
        (component_shift)renderer::renderer_component_bits::rigged_model_instance
};

void renderer::rigged_model_instance::set_animation_index(size_t i)
{
  _animation_index = i;
  animation_structures->clips[i].timeline.start_time = animation_structures->global_timeline.t;
}

size_t renderer::rigged_model_instance::animation_index() const
{
  return *_animation_index;
}

bool renderer::rigged_model_instance::has_animations() const
{
  return !animation_structures->clips.empty();
}

renderer::skeleton_animation_clip& renderer::rigged_model_instance::current_clip()
{
  return animation_structures->clips[*_animation_index];
}

renderer::animation_time renderer::rigged_model_instance::current_clip_time() const
{
  return animation_structures->clips[*_animation_index].timeline.t;
}

renderer::animation_time renderer::rigged_model_instance::current_clip_duration() const
{
  return animation_structures->clips[*_animation_index].timeline.duration;
}

void renderer::rigged_model_instance::increment_global_time(animation_time inc)
{
  animation_structures->global_timeline.t += inc;
}
renderer::animation_timelines renderer::rigged_model_instance::get_timelines()
{
  return {
    animation_structures->global_timeline,
    animation_structures->clips[*_animation_index].timeline
  };
}
