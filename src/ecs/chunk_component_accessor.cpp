//
// Created by sava on 10/23/19.
//

#include <ecs/chunk_component_accessor.hpp>

ecs::chunk_component_accessor::chunk_component_accessor(void *chunk_ptr, component_bitset archetype_id) :
    _chunk_ptr(chunk_ptr),
    _description(calculate_chunk_description(archetype_id, chunk_ptr)),
    _header(header(chunk_ptr, _description.header_offset))
{
    // init the headers
    ecs::component_ptr* cursor = _header;
    std::uint8_t* chunk_ptr_byte = (std::uint8_t*) _chunk_ptr;
    std::uint8_t* component_ptr = chunk_ptr_byte + _description.components_offset;
    
    for(std::uint8_t i = 0; i < ECS_MAX_COMPONENT_TYPES; ++i)
    {
        component_bitset bit = component_bitset(1) << i;
        if (!(archetype_id & bit))
            continue;

        auto meta = component_meta::bit_metas.find(i)->second;

        component_ptr = memory::align_pointer(component_ptr, meta.align());

        cursor->component_bitshift = i;
        cursor->component_offset = (std::uint32_t)(component_ptr - chunk_ptr_byte);
        cursor++;

        component_ptr += meta.size();
    }
}

ecs::chunk_description ecs::chunk_component_accessor::calculate_chunk_description(component_bitset archetype_id, void* chunk_ptr)
{
    auto component_ptr_size = sizeof(component_ptr);
    auto component_ptr_align = alignof(component_ptr);

    uintptr_t chunk_start = (uintptr_t) chunk_ptr;
    uintptr_t chunk_end = chunk_start;

    uintptr_t components_offset = -1;
    uintptr_t header_offset = -1;
    component_ptr* header;

    uint16_t component_count = 0;

    // first, do the component pointers
    for (std::uint32_t i = 0; i < ECS_MAX_COMPONENT_TYPES; ++i)
    {
        component_bitset bit = component_bitset(1) << i;
        if (!(archetype_id & bit))
            continue;

        component_count++;

        auto& meta = component_meta::bit_metas.find(i)->second;

        // we will have one component_ptr at the start
        chunk_end = memory::align_address(chunk_end, component_ptr_align);

        if (header_offset == -1)
        {
            header_offset = chunk_end - chunk_start;
        }

        chunk_end += component_ptr_size;
    }

    // now, the components
    for (std::uint8_t i = 0; i < ECS_MAX_COMPONENT_TYPES; ++i)
    {
        component_bitset bit = component_bitset(1) << i;
        if (!(archetype_id & bit))
            continue;

        auto& meta = component_meta::bit_metas.find(i)->second;

        chunk_end = memory::align_address(chunk_end, component_ptr_align);

        if (components_offset == -1)
            components_offset = chunk_end - chunk_start;

        chunk_end += meta.size();
    }

    chunk_end += PaddingHack;

    return { chunk_end - chunk_start, header_offset, components_offset, component_count };
}

ecs::chunk_description ecs::chunk_component_accessor::calculate_worst_case_chunk_description(component_bitset archetype_id)
{    
    auto component_ptr_size = sizeof(component_ptr);
    auto component_ptr_align = alignof(component_ptr);
    return calculate_chunk_description(archetype_id, reinterpret_cast<void*>(component_ptr_align + 1));
}

void ecs::chunk_component_accessor::construct()
{
    auto cursor = _header;
    auto count = _description.component_count;

    while (count-- > 0)
    {
        std::uint8_t* ptr_byte = (std::uint8_t*)_chunk_ptr;

        void *ptr = (void*)(ptr_byte + cursor->component_offset);
        auto meta = component_meta::bit_metas.find(cursor->component_bitshift)->second;
        meta.construct(ptr);
        cursor++;
    }
}

void ecs::chunk_component_accessor::destruct()
{
    auto cursor = _header;
    auto count = _description.component_count;

    while (count-- > 0)
    {
        std::uint8_t* ptr_byte = (std::uint8_t*)_chunk_ptr;

        void *ptr = (void*)(ptr_byte + cursor->component_offset);
        auto meta = component_meta::bit_metas.find(cursor->component_bitshift)->second;
        meta.destruct(ptr);
        cursor++;
    }
}

void *ecs::chunk_component_accessor::get_component(std::uint8_t component_bitshift)
{
    auto cursor = _header;
    auto count = _description.component_count;

    while (count-- > 0)
    {
        auto chunk_ptr_byte = (uintptr_t) _chunk_ptr;

        if (cursor->component_bitshift == component_bitshift)
        {
            return (void *) (chunk_ptr_byte + cursor->component_offset);
        }

        cursor++;
    }

    return nullptr;
}

ecs::component_ptr* ecs::chunk_component_accessor::header(void* chunk_ptr, uintptr_t header_offset)
{
    auto chunk = reinterpret_cast<uintptr_t>(chunk_ptr);
    auto header = chunk + header_offset;
    return reinterpret_cast<ecs::component_ptr*>(header);
}