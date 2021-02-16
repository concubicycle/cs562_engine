#include <physics/rigid_body.hpp>
#include <ecs/ecs_types.hpp>
#include <physics/component_bits.hpp>


template<> const component_shift ecs::component<physics::rigid_body>::component_bitshift = 
  (component_shift)physics::component_bits::rigid_body;