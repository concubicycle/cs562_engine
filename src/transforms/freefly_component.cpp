#include <transforms/freefly_component.hpp>
#include <ecs/ecs_types.hpp>
#include <transforms/transform_component_bits.hpp>


template<> const component_shift ecs::component<transforms::freefly_component>::component_bitshift = (component_shift)transforms::transform_component_bits::freefly_component;
