//
// Created by sava on 10/13/19.
//

#include <ecs/ecs_types.hpp>
#include <ecs/component_meta.hpp>
#include <map>

std::map<component_shift, ecs::component_meta> ecs::component_meta::bit_metas { };
std::map<std::string, component_shift> ecs::component_meta::type_to_bit{ };

