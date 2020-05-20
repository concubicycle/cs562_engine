//
// Created by sava on 10/7/19.
//

#ifndef ECS_ATTEMPT2_POOL_ALLOCATOR_TESTS_H
#define ECS_ATTEMPT2_POOL_ALLOCATOR_TESTS_H


#include <cstdio>
#include <vector>
#include <cstring>

#include <memory/pool_allocator.hpp>

typedef struct sample_struct
{
    float number_float = 123.f;
    int number_int = 123;
    char a = 'a';
    char b = 'b';
    char c = 'c';
    char *ptr = (char *) 0x0abc;

} sample_struct;


void test_memory_block(size_t size, size_t align);

void test_memory_blocks();

void test_prepare_memory();

void test_pool_ctor();

void test_allocate();

void test_allocate_expanding();

void test_allocate_free();

void test_construct_destruct();


int test_pool_allocator_all()
{

    test_construct_destruct();

    test_memory_blocks();
    test_prepare_memory();
    test_pool_ctor();
    test_allocate();
    test_allocate_expanding();
    test_allocate_free();

    return 0;
}


void test_construct_destruct()
{
    for (int i = 0; i < 10000; ++i)
    {
        allocators::pool_allocator allocator(sizeof(sample_struct), 500, alignof(sample_struct));

        for (int i = 0; i < 500; ++i)
        {
            auto ptr = allocator.allocate();
        }
    }
}


void test_allocate_free()
{
    allocators::pool_allocator allocator(sizeof(sample_struct), 500, alignof(sample_struct));
    std::vector<void *> pointers;

    for (int i = 0; i < 500; ++i)
    {
        auto ptr = allocator.allocate();
        pointers.push_back(ptr);
    }

    for (int i = 0; i < 500; ++i)
    {
        allocator.free_element(pointers[i]);
    }

    for (int i = 0; i < 500; ++i)
    {
        auto ptr = allocator.allocate();
    }

    if (allocator.memory_blocks().size() != 1)
        throw "Should still be 1 memory block after 500 allocations, then 500 frees, then 500 allocations.";
}


void test_allocate_expanding()
{
    allocators::pool_allocator allocator(sizeof(sample_struct), 500, alignof(sample_struct));

    for (int i = 0; i < 501; ++i)
    {
        allocator.allocate();
    }

    if (allocator.memory_blocks().size() != 2)
        throw "Should still be 2 memory block after 1000 allocations.";
}

void test_allocate()
{
    allocators::pool_allocator allocator(sizeof(sample_struct), 1000, alignof(sample_struct));

    for (int i = 0; i < 1000; ++i)
    {
        auto ptr = allocator.allocate();
        memset(ptr, 0, sizeof(sample_struct));
    }

    if (allocator.memory_blocks().size() != 1)
        throw "Should still be only 1 memory block after 1000 allocations.";
}

void test_pool_ctor()
{
    allocators::pool_allocator allocator(sizeof(sample_struct), 1000, alignof(sample_struct));

    auto &blocks = allocator.memory_blocks();

    if (blocks.size() != 1)
    {
        throw "blocks should have size 1 after constructor";
    }
}

void test_memory_blocks()
{
    int i = 0;
    printf("Testing memory block initialization...\n");
    for (int size = 8; size < 4096; ++size)
    {
        for (int align = 2; align < 1024; align *= 2)
        {
            test_memory_block(size, align);
            i++;
        }
    }
    printf("\nDone! Tested %d size/align combos.\n", i);
}


void test_memory_block(size_t size, size_t align)
{
    allocators::pool_memory_block block(size, 100, align);

    uintptr_t last_written = (uintptr_t) block.last_addr_written();
    uintptr_t original_ptr = (uintptr_t) block.last_addr_written();
    uintptr_t aligned_ptr = (uintptr_t) block.aligned_pointer();
    size_t block_size_bytes = block.size();

    if (original_ptr + block_size_bytes < last_written)
    {
        printf("Wrote past the block size! size: %d, align: %d\n", size, align);
        throw "Wrote past the block size!";
    }

    if (aligned_ptr % align != 0)
    {
        printf("Unaligned pointer! size: %d, align: %d\n", size, align);
        throw "Unaligned pointer!";
    }
}

void test_prepare_memory()
{
    auto size = sizeof(sample_struct);
    auto align = alignof(sample_struct);

    allocators::pool_memory_block block(size, 100, align);

    std::uint8_t *cursor = block.aligned_pointer();

    // should have a 0 ptr at start of 99th chunk
    int i;
    for (i = 0; i < 99; i++)
    {
        std::uint8_t *last_cursor = cursor;
        cursor = *((std::uint8_t **) cursor);

        uintptr_t addr = (uintptr_t) cursor;
        uintptr_t last_addr = (uintptr_t) last_cursor;

        if (addr - last_addr != sizeof(sample_struct))
        {
            auto diff = addr - last_addr;
            printf("Offset between chunks isn't sizeof(sample_struct). size: %d, diff: %d", size, diff);
            throw "Offset between chunks isn't sizeof(sample_struct)";
        }
    }

    cursor = block.aligned_pointer() + 99 * (sizeof(sample_struct));

    if (*((std::uint8_t **) cursor) != nullptr)
    {
        throw "Last chunk not pointing to terminating null";
    }
}


#endif //ECS_ATTEMPT2_POOL_ALLOCATOR_TESTS_H
