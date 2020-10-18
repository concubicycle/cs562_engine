//
// Created by sava on 10/6/19.
//

#ifndef ECS_ATTEMPT2_POOL_MEMORY_BLOCK_HPP
#define ECS_ATTEMPT2_POOL_MEMORY_BLOCK_HPP

#include <cstdint>
#include <cstdlib>

namespace allocators
{

/**
 * A memory pool block, which stores a linked list of chunks.
 * Each chunk is an element allocated/freed by the pool.
 */
    class pool_memory_block
    {
    public:
        pool_memory_block(size_t chunk_size, size_t chunk_count, uintptr_t alignment);

        pool_memory_block(pool_memory_block &&other) noexcept;

        // forbid copy and assignment for now.. if stl containers start freaking out, implement later.
        pool_memory_block(const pool_memory_block &other) = delete;

        pool_memory_block &operator=(const pool_memory_block &other) = delete;


        ~pool_memory_block();

        std::uint8_t *original_pointer() const { return _original_pointer; }

        std::uint8_t *aligned_pointer() const { return _aligned_pointer; }

        std::uint8_t *last_addr_written() const { return _last_addr_written; }

        size_t size() { return _malloc_size; }

        void link_last_chunk_to(const pool_memory_block &other);

    private:
        std::uint8_t *_original_pointer = nullptr;
        std::uint8_t *_aligned_pointer = nullptr;
        std::uint8_t *_last_addr_written;
        size_t _malloc_size;
        size_t _chunk_count;
        size_t _chunk_size;

        void prepare_memory(std::uint8_t *mem);

        size_t calc_total_malloc_size(uintptr_t alignment) const;        

        std::uint8_t *last_chunk_ptr();
    };
}

#endif //ECS_ATTEMPT2_POOL_MEMORY_BLOCK_HPP
