#include <renderer/timeline.hpp>

renderer::timeline::timeline()
  : start_time(0)
{
}

renderer::timeline::timeline(animation_time _start_time)
  : start_time(_start_time)
{
}

renderer::animation_time renderer::timeline::from_local_time(const timeline& child)
{
  return child.start_time + (1.f / playback_rate) * child.t;
}

void renderer::timeline::update(animation_time dt)
{
  t += dt * playback_rate;
}



/*******************/
renderer::local_timeline::local_timeline(animation_time d)
  : duration(d)
{
}

void renderer::local_timeline::update_local_time(const timeline& parent, bool loop)
{
  t = (parent.t - start_time) * playback_rate;

  if (t > duration && loop)
  {
    start_time = parent.t;
    t = t - duration;
  }
  else if (t > duration)
  {
    t = animation_time(1.f);
  }
}