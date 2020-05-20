//
// Created by sava on 1/25/20.
//
#include <ecs/ecs_types.hpp>
#include <transforms/transform_component_bits.hpp>
#include <transforms/transform.hpp>


template<> const component_shift ecs::component<transforms::transform>::component_bitshift = (component_shift)transforms::transform_component_bits::transform;
