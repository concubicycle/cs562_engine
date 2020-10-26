#ifndef __TIMELINE_HPP_
#define __TIMELINE_HPP_

#include <optional>
#include <renderer/animation_time.hpp>

namespace renderer
{
  struct timeline
  {
    float playback_rate{ 1 };
    animation_time t{ 0 };
    animation_time start_time;

    timeline();
    timeline(animation_time _start_time);

    animation_time from_local_time(const timeline& child);
    void update(animation_time dt);
  };

  struct local_timeline : public timeline
  {    
    animation_time duration;

    local_timeline(animation_time d);

    void update_local_time(const timeline& parent, bool loop);
  };

  
}

#endif