#include "curve_walker_system.hpp"
#include <renderer/rigged_model_instance.hpp>
#include <renderer/animator.hpp>
#include <transforms/transform.hpp>
#include "curve_walker.hpp"

curve_walker_system::curve_walker_system(core::frame_timer& time)
  : _time(time)

{
}

void curve_walker_system::initialize(ecs::state& state)
{
  state.each<curve_walker>([&](curve_walker& cw) {
    cw.walkee = state.first<renderer::rigged_model_instance>();
  });
}

void curve_walker_system::update(ecs::state& state)
{
  using namespace transforms;
  using namespace renderer;
  
  state.each<curve_walker>(
    [&](curve_walker& cw)
    {
      auto& walkee_transform = cw.walkee->get_component<transform>();
      auto& walkee_rigged_model = cw.walkee->get_component<renderer::rigged_model_instance>();


    });
}
