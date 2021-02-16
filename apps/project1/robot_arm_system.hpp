#pragma once

#include <ecs/system_base.hpp>
#include <core/frame_timer.hpp>
#include <asset/scene_hydrater.hpp>

class robot_arm_system : public ecs::system_base
{
public:
  explicit robot_arm_system(core::frame_timer& timer);

  void update(ecs::state& state) override;

private:
  core::frame_timer& _timer;
};