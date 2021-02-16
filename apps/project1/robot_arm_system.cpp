#include "robot_arm_system.hpp"

#include <renderer/rigged_model_instance.hpp>



robot_arm_system::robot_arm_system(core::frame_timer& timer) : _timer(timer)
{
}

void robot_arm_system::update(ecs::state& state)
{
  state.each<transforms::transform, renderer::rigged_model_instance>(
    [&](transforms::transform& t, renderer::rigged_model_instance& rmi)
    {
      if (rmi.ik_solver)
      {
        rmi.ik_solver->set_target(rmi.build_target());
        rmi.ik_solver->update(_timer.smoothed_delta_seconds_f());
      }
    });
}
