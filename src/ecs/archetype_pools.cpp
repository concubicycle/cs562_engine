//
// Created by sava on 10/15/19.
//

#include <cstdint>

#include <ecs/archetype_pools.hpp>

const std::uint32_t ecs::archetype_pools::MinArchetypeChunks = 512;

ecs::entity ecs::archetype_pools::make_entity(component_bitset archetype_id, entity_id id)
{
    auto& pool = find_pool(archetype_id);
    return pool.allocate_entity(id);
}


void ecs::archetype_pools::free_entity(ecs::entity& e)
{
    auto id = e.archetype_id();
    auto it = _archetype_pools.find(id);

    if (it == _archetype_pools.end())
    {
        std::cerr << "Trying to remove entity with untracked archetype" << std::endl;
        assert(false);
    }

    return _archetype_pools.find(id)->second.free_entity(e);
}

ecs::archetype_pool& ecs::archetype_pools::find_pool(component_bitset archetype_id)
{
    auto it = _archetype_pools.find(archetype_id);

    if (it == _archetype_pools.end())
    {
        _archetype_pools.emplace(std::piecewise_construct,
            std::forward_as_tuple(archetype_id),
            std::forward_as_tuple(archetype_id, find_min_allocation(archetype_id)));
    }

    return _archetype_pools.find(archetype_id)->second;
}

std::uint32_t ecs::archetype_pools::find_min_allocation(component_bitset archetype_id)
{
    std::uint32_t max = 0;

    for (std::uint8_t i = 0; i < ECS_MAX_COMPONENT_TYPES; ++i)
    {
        component_bitset bit = component_bitset(1) << i;
        
        if (!(archetype_id & bit))
            continue;

        auto& meta = component_meta::bit_metas.find(i)->second;
        max = std::max(max, meta.min_allocation_count());        
    }

    return max;
}