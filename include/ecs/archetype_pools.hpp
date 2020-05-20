//
// Created by sava on 10/8/19.
//

#ifndef __ENTITY_FACTORY_H_
#define __ENTITY_FACTORY_H_

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <cstdint>

#include <ecs/query_cache.hpp>

#include "ecs_types.hpp"
#include "archetype_pool.hpp"
#include "archetype_id.hpp"



namespace ecs
{
    class archetype_pools
    {
        static const std::uint32_t MinArchetypeChunks;

    public:
        archetype_pools() {}
        archetype_pools(const archetype_pools& other) = delete;
        archetype_pools &operator=(const archetype_pools &other) = delete;

        entity make_entity(component_bitset archetype_id, entity_id id);
        void free_entity(entity& e);

        template <class TComponent>
        entity add_component(entity& e)
        {
            auto new_archetype = component<TComponent>::archetype_bit | e.archetype_id();
            auto& new_pool = find_pool(new_archetype);
            auto new_entity = new_pool.allocate_entity(e.id());
            new_entity.copy_components_from(e);

            auto& old_pool = find_pool(e.archetype_id());
            old_pool.free_entity(e);

            return new_entity;
        }

        
    private:
        std::unordered_map<component_bitset, archetype_pool> _archetype_pools;
       
        archetype_pool& find_pool(component_bitset archetype_id);
        static std::uint32_t find_min_allocation(component_bitset archetype_id);
    };
}

#endif //__ENTITY_FACTORY_H_
