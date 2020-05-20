//
// Created by sava on 10/7/19.
//

#ifndef ECS_ATTEMPT2_ECS_H
#define ECS_ATTEMPT2_ECS_H

#include <vector>

#include "ecs_types.hpp"
#include <memory/pool_allocator.hpp>

#include "component_meta.hpp"
#include "chunk_component_accessor.hpp"
#include "entity.hpp"
#include "archetype_chunk_component.hpp"

namespace ecs
{
    /**
     * This wraps a raw memory pool, and hands out entities. Entities contain accessor objects,
     * which know how to get the components from the raw chunk of entity memory.
     */
    class archetype_pool
    {
    public:
        archetype_pool(component_bitset arch_id, std::uint32_t count);
        entity allocate_entity(entity_id id);
        void free_entity(entity& entity);
        void free_entity(entity_id entity_id);     

    private:
        component_bitset _arch_id;        
        allocators::pool_allocator _allocator;

        static size_t calc_chunk_size(component_bitset archetype_id)
        {
            return chunk_component_accessor::calculate_worst_case_chunk_description(archetype_id).chunk_size;
        }
    };

} // namespace ecs

#endif //ECS_ATTEMPT2_ECS_H
