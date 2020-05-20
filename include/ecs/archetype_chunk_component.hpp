//
// Created by sava on 10/8/19.
//

#ifndef ECS_ATTEMPT2_ARCHETYPE_CHUNK_COMPONENT_H
#define ECS_ATTEMPT2_ARCHETYPE_CHUNK_COMPONENT_H

#include <map>

#include "ecs_types.hpp"
#include "component_meta.hpp"

namespace ecs
{

    typedef struct archetype_chunk_component
    {
        archetype_chunk_component(std::uint32_t offset, component_meta &m) :
            chunk_component_offset(offset),
            meta(m)
        {
        }

        /**
         * The offset given a void* of the component within an archetype chunk of memory.
         */
        std::uint32_t chunk_component_offset;

        /**
         * Metadata for a component type.
         */
        component_meta &meta;
    } archetype_chunk_descriptor;


}

#endif //ECS_ATTEMPT2_ARCHETYPE_CHUNK_COMPONENT_H
