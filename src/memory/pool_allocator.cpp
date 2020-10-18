#include <memory/pool_allocator.hpp>

allocators::pool_allocator::pool_allocator(
    size_t chunk_size,
    size_t chunk_count,
    uintptr_t alignment) :
    _chunk_size(chunk_size > sizeof(void *) ? chunk_size : sizeof(void *)),
    _next_expand(chunk_count * ExpandGrowFactor),
    _total_allocated(0),
    _alignment(alignment)
{
    if (_alignment < alignof(void*))
        _alignment = alignof(void*);

    if (_chunk_size * chunk_count > MaxAllocSize)
    {
        _next_expand = MaxAllocSize / _chunk_size;
    }

    // for an 8 chunk block, we will allocate new block when trying to get the penultimate chunk.
    // have the pool grab one extra chunk to actually have chunk_count capacity, before an allocation
    // is triggered.
    _memory_blocks.emplace_back(_chunk_size, chunk_count + 1, _alignment);
    _head = _memory_blocks[0].aligned_pointer();
}


void allocators::pool_allocator::linked_expand(size_t chunk_count)
{
    _memory_blocks.emplace_back(_chunk_size, chunk_count, _alignment);
    link_last_two_mem_blocks();
}

void *allocators::pool_allocator::allocate()
{
    std::uint8_t *next = *((std::uint8_t **) _head);

    if (!next)
    {
        linked_expand(_next_expand);
        next = _memory_blocks.back().aligned_pointer();

        if ((_next_expand * _chunk_size) < MaxAllocSize)
            _next_expand *= ExpandGrowFactor;
    }

    std::uint8_t *current_block = _head;
    _head = next;

    _total_allocated++;
    return current_block;
}


void allocators::pool_allocator::free_element(void *ptr)
{
    if (!ptr)
        return;

    *((std::uint8_t **) ptr) = _head;
    _head = (std::uint8_t *) ptr;

    _total_allocated--;
}

void allocators::pool_allocator::link_last_two_mem_blocks()
{
    auto &last_block = *(_memory_blocks.rbegin() + 1);
    auto &this_block = *_memory_blocks.rbegin();
    last_block.link_last_chunk_to(this_block);
}
