#ifndef __PHYS_UPDATE_HPP_
#define __PHYS_UPDATE_HPP_

#include <ecs/system_base.hpp>
#include <core/frame_timer.hpp>


namespace physics
{
  class physics_update : public ecs::system_base
  {
  public:
    explicit physics_update(core::frame_timer& timer);

    void initialize(ecs::state& state) override;
    void update(ecs::state& state) override;

  private:
    core::frame_timer& _timer;
  };

}


#endif
