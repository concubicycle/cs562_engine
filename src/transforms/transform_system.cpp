#include <transforms/transform_system.hpp>


void transforms::transform_system::update(ecs::state& state)
{
	state.each<transform>([&](transform& t) {
		if (t.is_dirty())
			update_transform(state, t);
	});
}

void transforms::transform_system::update_transform(ecs::state& state, transform& t)
{
	t.calculate_local();
	auto parent_id = t.parent_id();
	if (parent_id)
	{
		auto& parent = state.find_entity(*parent_id);
		auto& parent_t = parent.get_component<transform>();
		update_transform(state, parent_t);
		auto& t_local_to_world = t.local_to_world();
		t_local_to_world = std::as_const(parent_t).local_to_world() * std::as_const(t).local_to_parent();
	}

	t.set_clean();
}
