#include <iostream>

#include <physics/physics_update.hpp>

#include <transforms/transform.hpp>
#include <physics/rigid_body.hpp>

physics::physics_update::physics_update(core::frame_timer& timer)
  : _timer(timer)
{
}

void physics::physics_update::initialize(ecs::state& state)
{
  using namespace transforms;

  state.each<transform, rigid_body>([&](transform& t, rigid_body& rb)
    {
      rb.model->add_torque(Eigen::Vector3f::UnitZ() * 100);
      std::cout << rb.model->points() << std::endl;

      rb.model->set_position(t.world_position());
      rb.model->set_rotation(t.rotation());
    });
}

void physics::physics_update::update(ecs::state& state)
{
  using namespace transforms;

  state.each<transform, rigid_body>([&](transform& t, rigid_body& rb)
    {
      auto dt = _timer.delta_seconds();
      if (dt == core::float_seconds::zero()) return;

      //rb.model->add_force(Eigen::Vector3f(0, -rb.gravity, 0));

      rb.model->rk4_step(_timer.delta_seconds());
      t.rotation() = rb.model->rotation();
      auto& translation = t.position();
      translation = Eigen::Translation3f(rb.model->position());
    });
}
