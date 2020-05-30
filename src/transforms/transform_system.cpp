#include <transforms/transform_system.hpp>


void transforms::transform_system::initialize(ecs::state& state)
{
	state.each<transform>([&](transform& t) {
		update_transform(state, t);
	});
}

void transforms::transform_system::update(ecs::state& state)
{
	state.each<transform>([&](transform& t) {		
		update_transform(state, t);
	});
}

void transforms::transform_system::set_children_dirty(ecs::state& state, transform& t)
{
	for (auto& child_id_ptr : t.children())
	{
		if (!state.has_entity(child_id_ptr.first))
		{
			t.remove_child(child_id_ptr.first);
		}
		else
		{
			auto& child_t = child_id_ptr.second->get_component<transform>();
			child_t.set_dirty();
			set_children_dirty(state, child_t);
		}
	}
}

void transforms::transform_system::update_transform(ecs::state& state, transform& t)
{	
	if (!t.is_dirty()) return;

	set_children_dirty(state, t);

	t.calculate_local();

	auto parent = std::as_const(t).parent();
	if (parent)
	{			
		auto& parent_t = (*parent)->get_component<transform>();
		update_transform(state, parent_t);
		t.calculate_local_to_world(std::as_const(parent_t).local_to_world());
	}
	else
	{
		t.calculate_local_to_world();
	}

	t.set_clean();
}
