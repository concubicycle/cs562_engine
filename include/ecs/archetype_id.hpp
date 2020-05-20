//
// Created by sava on 9/21/19.
//

#ifndef ECS_DEV_ARCHETYPE_ID_H
#define ECS_DEV_ARCHETYPE_ID_H

#include <cstdint>
#include "ecs_types.hpp"

namespace ecs
{

template <class T>
constexpr const component_bitset archetype_id()
{
    return T::archetype_bit;
}

template <class T, class U, class... TRest>
constexpr const component_bitset archetype_id()
{
    return T::archetype_bit | archetype_id<U, TRest...>();
}

} // namespace ecs

#endif //ECS_DEV_ARCHETYPE_ID_H
