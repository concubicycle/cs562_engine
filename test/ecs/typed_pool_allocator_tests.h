//
// Created by sava on 10/7/19.
//

#ifndef ECS_ATTEMPT2_TYPED_POOL_ALLOCATOR_TESTS_H
#define ECS_ATTEMPT2_TYPED_POOL_ALLOCATOR_TESTS_H


#include <cstdio>
#include <vector>

#include <memory/pool_allocator.hpp>
#include <memory/typed_pool_allocator.hpp>

typedef struct another_struct
{
    int foo;
    int bar;
    int baz;
    unsigned long l;
} another_struct;

typedef struct sample_struct_typed
{
    float number_float = 123.f;
    int number_int = 123;
    char a = 'a';
    char b = 'b';
    char c = 'c';
    char *ptr = (char *) 0x0abc;

    another_struct s;
} sample_struct_t;


void test_pool_ctor_typed();

void test_allocate_typed();

void test_allocate_expanding_typed();

void test_allocate_free_typed();


int test_typed_pool_allocator_all()
{
    test_pool_ctor_typed();
    test_allocate_typed();
    test_allocate_expanding_typed();
    test_allocate_free_typed();

    return 0;
}


void test_allocate_free_typed()
{
    allocators::typed_pool_allocator<sample_struct_typed> allocator(500);
    std::vector<sample_struct_typed *> pointers;

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


void test_allocate_expanding_typed()
{
    allocators::typed_pool_allocator<sample_struct_typed> allocator(500);

    for (int i = 0; i < 501; ++i)
    {
        allocator.allocate();
    }

    if (allocator.memory_blocks().size() != 2)
        throw "Should still be 2 memory block after 1000 allocations.";
}

void test_allocate_typed()
{
    allocators::typed_pool_allocator<sample_struct_typed> allocator(1000);

    for (int i = 0; i < 1000; ++i)
    {
        allocator.allocate();
    }

    if (allocator.memory_blocks().size() != 1)
        throw "Should still be only 1 memory block after 1000 allocations.";
}

void test_pool_ctor_typed()
{
    allocators::typed_pool_allocator<sample_struct_typed> allocator(1000);

    auto &blocks = allocator.memory_blocks();

    if (blocks.size() != 1)
    {
        throw "blocks should have size 1 after constructor";
    }
}


#endif //ECS_ATTEMPT2_POOL_ALLOCATOR_TESTS_H
