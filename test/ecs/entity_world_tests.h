#include <cstdio>
#include <cstring>

#include <ecs/ecs_types.hpp>
#include <ecs/world.hpp>
#include <ecs/system_base.hpp>

#include "components.h"

class system_a : public ecs::system_base
{
public:
	void update(ecs::state& state) override
	{
		update_count++;
	}

	int update_count = 0;
};

class system_b : public ecs::system_base
{
public:
	void update(ecs::state& state) override
	{
		update_count++;
	}

	int update_count = 0;
};

void test_update();

int test_ecs_world_all()
{
	test_update();
	printf("Done with ecs world tests.\n");
	return 0;
}

void test_update()
{
	ecs::archetype_pools factory;
	ecs::state state(factory);

	system_a a;
	system_b b;

	ecs::systems systems ({ &a, &b });

	ecs::world world(systems, state);

	world.update();

	if (a.update_count == 0 || b.update_count == 0)
	{
		throw std::runtime_error("failed to update...?");
	}
}