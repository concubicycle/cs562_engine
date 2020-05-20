//
// Created by sava on 11/21/19.
//

#include <ecs/entity.hpp>

ecs::entity::entity(entity_id id, component_bitset archetype_id, std::uint8_t *chunk_ptr) :
    _id(id),
    _accessor(chunk_ptr, archetype_id),
    _active(true),
    _archetype_id(archetype_id)
{
    _accessor.construct();
}

ecs::entity::entity(const ecs::entity &other):
    _id(other._id),
    _accessor(other._accessor),
    _active(other._active),
    _archetype_id(other._archetype_id)
{
}

void ecs::entity::destroy()
{
    _accessor.destruct();
    _active = false;
}

void ecs::entity::copy_components_from(ecs::entity &other)
{
    for(std::uint8_t i = 0; i < ECS_MAX_COMPONENT_TYPES; ++i)
    {
        component_bitset bit = component_bitset(1) << i;

        if (!has(bit) || !other.has(bit)) continue;

        auto& meta = component_meta::bit_metas.find(i)->second;

        void* destination = _accessor.get_component(i);
        void* source = other._accessor.get_component(i);

        meta.copy(destination, source);
    }
}