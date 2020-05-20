//
// Created by sava on 10/6/19.
//

#include <cassert>
#include <memory/pool_memory_block.hpp>
#include <memory/alignment.hpp>


allocators::pool_memory_block::pool_memory_block(
    std::uint32_t chunk_size,
    std::uint32_t chunk_count,
    uintptr_t alignment) :
    _chunk_size(chunk_size),
    _chunk_count(chunk_count),
    _last_addr_written(nullptr)
{
    assert (chunk_size > 0 && chunk_count > 0);

    _malloc_size = calc_total_malloc_size(alignment);
    _original_pointer = (std::uint8_t *) malloc(_malloc_size);
    _aligned_pointer = memory::align_pointer(_original_pointer, alignment);

    prepare_memory(_aligned_pointer);
}

allocators::pool_memory_block::pool_memory_block(allocators::pool_memory_block &&other) noexcept
    :
    _chunk_size(other._chunk_size),
    _chunk_count(other._chunk_count),
    _malloc_size(other._malloc_size),
    _original_pointer(other._original_pointer),
    _aligned_pointer(other._aligned_pointer),
    _last_addr_written(other._last_addr_written)
{
    other._original_pointer = nullptr;
}


allocators::pool_memory_block::~pool_memory_block()
{
    if (_original_pointer != nullptr)
        free(_original_pointer);
}


/**
 *
 * Prepare a recently allocated chunk of memory for use as a pool by setting each chunk
 * to point to the next chunk, and the last one to point to nullptr.
 *
 * Say, for chunk size of 8, chunk count 8:
 * Memory that starts at 0x0004 raw and 0x0008 aligned
 * total size = 72 (8 * 8 + 8 or size*count + alignment).
 *
 * We iterate 7 chunks, or 56 bytes:
 * 0. 0x0008 -> 0x0010
 * 1. 0x0010 -> 0x0018
 * 2. 0x0018 -> 0x0020
 * 3. 0x0018 -> 0x0028
 * 4. 0x0028 -> 0x0030
 * 5. 0x0030 (48) -> (56) 0x0038
 * 6. 0x0038 (56) -> 0x0040 (64)
 *
 * last_chunk_offset = 8 * 7 = 58
 * last_chunk_ptr = 72
 * 64 -> nullptr
 *
 * memory ends at: 84 (original address is 4).
 *
 */
void allocators::pool_memory_block::prepare_memory(std::uint8_t *mem)
{
    for (std::uintptr_t chunk_index = 0; chunk_index < _chunk_count - 1; ++chunk_index)
    {
        std::uint8_t *chunk = mem + (chunk_index * (std::uintptr_t)_chunk_size);
        *((std::uint8_t **) chunk) = chunk + _chunk_size;
    }

    std::uint8_t *last_chunk = last_chunk_ptr();
    *((std::uint8_t **) last_chunk) = nullptr;

    _last_addr_written = last_chunk;
}

/**
 * From previous prepare_memory()'s example, this returns _aligned_pointer + (7 * 8) = 64 (0x0040)
 * @return pointer to last chunk in block
 */
std::uint8_t *allocators::pool_memory_block::last_chunk_ptr()
{
    std::uint32_t last_chunk_offset = (_chunk_count - 1) * _chunk_size;
    return _aligned_pointer + last_chunk_offset;
}

void allocators::pool_memory_block::link_last_chunk_to(const allocators::pool_memory_block &other)
{
    std::uint8_t *last_chunk = last_chunk_ptr();
    *((std::uint8_t **) last_chunk) = other._aligned_pointer;
}

size_t allocators::pool_memory_block::calc_total_malloc_size(uintptr_t alignment) const
{
    return ((size_t)_chunk_size * _chunk_count) + alignment;
}
