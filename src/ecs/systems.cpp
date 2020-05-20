#include <ecs/systems.hpp>
#include <ecs/state.hpp>



ecs::systems::systems(std::vector<system_base *> systems): _systems(std::move(systems)) {}


void ecs::systems::update(ecs::state& state)
{
	for (auto* sys : _systems)
		sys->update(state);
}

void ecs::systems::initialize(ecs::state &state)
{
    for (auto* sys : _systems)
        sys->initialize(state);
}

void ecs::systems::cleanup(ecs::state& state)
{
	for (auto* sys : _systems)
		sys->cleanup(state);
}

