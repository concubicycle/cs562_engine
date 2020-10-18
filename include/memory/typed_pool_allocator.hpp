#ifndef __TYPED_POOL_ALLOCATOR_H_
#define __TYPED_POOL_ALLOCATOR_H_

#include "pool_allocator.hpp"
#include "pool_memory_block.hpp"

namespace allocators
{
    template<typename T, typename... TArgs>
    class typed_pool_allocator
    {
    public:
        typed_pool_allocator(std::uint32_t chunk_count) :
            raw_allocator(sizeof(T), chunk_count, alignof(T))
        {
        }

        T *allocate(TArgs... ctr_params)
        {
            auto raw_ptr = raw_allocator.allocate();
            auto t_ptr = new(raw_ptr) T(ctr_params...);
            return t_ptr;
        }

        void free_element(T *element)
        {
            element->~T();
            raw_allocator.free_element((void *) element);
        }

        const std::vector<pool_memory_block> &memory_blocks() { return raw_allocator.memory_blocks(); }


    private:
        pool_allocator raw_allocator;
    };
} // namespace allocators

#endif
