#include <cstdio>
#include <cstring>

#include <ecs/ecs_types.hpp>
#include <ecs/world.hpp>

#include "components.h"

void test_basic();
void test_each();
void test_find();


int test_ecs_state_all()
{
    ecs::register_component<transform>("transform");
    ecs::register_component<renderdata>("renderdata");

    test_basic();
    test_each();
    test_find();
   
    printf("Done with ecs state tests.\n");
    return 0;
}

void test_basic()
{
    unsigned int  count = 100;

    while (count-- > 0)
    {
        ecs::archetype_pools factory;
        ecs::state state(factory);

        auto& entityA = state.add_entity<transform, renderdata>();
        auto& entityB = state.add_entity<transform>();

        auto& transformA = entityA.get_component<transform>();
        auto& transformB = entityB.get_component<transform>();
        auto& renderA = entityA.get_component<renderdata>();
    }
}

void test_each()
{    
    unsigned int  count = 100;

    while (count-- > 0)
    {
        ecs::archetype_pools factory;
        ecs::state state(factory);

        auto& entityA = state.add_entity<transform, renderdata>();
        auto& entityB = state.add_entity<transform, renderdata>();
        auto& entityC = state.add_entity<transform>();

        auto& transformA = entityA.get_component<transform>();
        auto& transformB = entityB.get_component<transform>();
        auto& renderA = entityA.get_component<renderdata>();

        int iter_count = 0;

        state.each<transform, renderdata>([&iter_count](transform& t, renderdata& r) {
            iter_count++;
            });

        if (iter_count != 2) throw std::runtime_error("iteration is wrong");

        iter_count = 0;

        state.each<transform>([&iter_count](transform& t) {
            iter_count++;
            t.pitch = 123;
            });

        if (iter_count != 3) throw std::runtime_error("iteration is wrong");

        state.each<transform>([&iter_count](transform& t) {
            if (t.pitch != 123)
                throw std::runtime_error("transform pitch values should all be 123");
            });
    }
}

void test_find()
{
    unsigned int  count = 100;

    while (count-- > 0)
    {
        ecs::archetype_pools factory;
        ecs::state state(factory);

        auto& entityA = state.add_entity<transform, renderdata>(111);
        auto& entityB = state.add_entity<transform, renderdata>(222);
        auto& entityC = state.add_entity<transform>(333);

        auto& a = state.find_entity(111);
        auto& b = state.find_entity(222);
        auto& c = state.find_entity(333);

        a.get_component<transform>().pitch = 1;
        b.get_component<transform>().pitch = 1;
        c.get_component<transform>().pitch = 1;
    }
}