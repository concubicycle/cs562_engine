#include <ecs/archetype_pool.hpp>

ecs::archetype_pool::archetype_pool(component_bitset arch_id, std::uint32_t count) :
    _arch_id(arch_id),
    _allocator(
        calc_chunk_size(_arch_id),
        count,
        chunk_component_accessor::chunk_align())
{
}

ecs::entity ecs::archetype_pool::allocate_entity(entity_id id)
{
    std::uint8_t* ptr = (std::uint8_t*) _allocator.allocate();
    return entity(id, _arch_id, ptr);
}

void ecs::archetype_pool::free_entity(entity& entity)
{
    entity.destroy();
    _allocator.free_element(entity.ptr());
}

