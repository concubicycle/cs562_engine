//
// Created by sava on 9/21/19.
//

#ifndef ECS_DEV_ECS_TYPES_H
#define ECS_DEV_ECS_TYPES_H

#include <cstdint>
#include <bitset>

#define ECS_MAX_COMPONENT_TYPES 64

typedef unsigned long long component_bitset;

typedef std::uint32_t entity_id;
typedef std::uint8_t component_shift;

namespace ecs
{
	const entity_id default_entity_id{};
}


#endif //ECS_DEV_ECS_TYPES_H
