//
// Created by sava on 10/15/19.
//

#include <ecs/entity.hpp>
#include <ecs/state.hpp>

ecs::state::state(archetype_pools& state) :
    _entity_pools(state)
{
}

ecs::entity& ecs::state::add_entity(component_bitset archetype_id)
{
    return add_entity(archetype_id, NextEntityId++);;
}

ecs::entity& ecs::state::add_entity(component_bitset archetype_id, entity_id id)
{
    _entity_lookup.emplace(id, _entity_pools.make_entity(archetype_id, id));
    auto& new_entity = _entity_lookup.find(id)->second;

    for (auto& pair : _caches)
    {
        if ((archetype_id & pair.first) == pair.first)
        {
            pair.second.accessors.emplace_back(new_entity.id(), new_entity.accessor(), new_entity.archetype_id());
            pair.second.sort();
        }
    }

    while (_entity_lookup.find(NextEntityId) != _entity_lookup.end())
        NextEntityId++;

    return new_entity;
}

void ecs::state::remove_entity(entity& entity)
{
    auto archetype_id = entity.archetype_id();

    _ids.erase(entity.id());
    _entity_pools.free_entity(entity);

    for (auto& pair : _caches)
    {
        if ((archetype_id & pair.first) == pair.first)
        {
            pair.second.remove(entity.id());
            pair.second.sort();
        }
    }

    auto it = _entity_lookup.find(entity.id());
    _entity_lookup.erase(it);
}

void ecs::state::free_all()
{
    for (auto& pair : _entity_lookup)
        _entity_pools.free_entity(pair.second);

    _entity_lookup.clear();
    _caches.clear();
    _ids.clear();
}

ecs::query_cache& ecs::state::find_query_cache(component_bitset archetype)
{
    if (_caches.find(archetype) == _caches.end())
    {
        _caches.emplace(std::piecewise_construct,
            std::forward_as_tuple(archetype),
            std::forward_as_tuple(build_query_cache(archetype)));
    }

    return _caches.find(archetype)->second;
}

ecs::query_cache ecs::state::build_query_cache(component_bitset archetype)
{
    query_cache cache{ archetype };

    for (auto& pair : _entity_lookup)
    {
        auto& e = pair.second;

        if ((e.archetype_id() & archetype) == archetype)
            cache.accessors.emplace_back(e.id(), e.accessor(), e.archetype_id());
    }

    cache.sort();
    return cache;
}

ecs::entity& ecs::state::find_entity(entity_id id)
{
    if (_entity_lookup.find(id) == _entity_lookup.end())
    {
        int i = 0;
        i++;
    }

    return _entity_lookup.find(id)->second;
}

bool ecs::state::has_entity(entity_id id)
{
    return _entity_lookup.find(id) != _entity_lookup.end();
}

std::atomic_uint ecs::state::NextEntityId = 100000;
