/*Implementation based on Game Engine Programming*/

#ifndef __POOL_ALLOCATOR_H_
#define __POOL_ALLOCATOR_H_

#include <stdlib.h>
#include <vector>

#include "pool_memory_block.hpp"


namespace allocators
{
    class pool_allocator
    {
        // *really* don't want to expand too often.
        const static int ExpandGrowFactor = 3;
        const static int MaxAllocSize = 4194304;

    public:
        pool_allocator(size_t chunk_size, size_t chunk_count, uintptr_t alignment);

        pool_allocator(const pool_allocator &other) = delete;

        size_t size() const { return _total_allocated; }

        void *allocate();

        void free_element(void *ptr);

        void linked_expand(size_t size_bytes);

        const std::vector<pool_memory_block> &memory_blocks() { return _memory_blocks; }


    private:
        std::vector<pool_memory_block> _memory_blocks;

        uintptr_t _alignment;
        std::uint8_t *_head;
        size_t _chunk_size;
        size_t _total_allocated;
        size_t _next_expand;

        void link_last_two_mem_blocks();
    };

} // namespace allocators

#endif