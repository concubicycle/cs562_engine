#include "curve_walker.hpp"
#include "app_component_bits.hpp"
#include <transforms/transform.hpp>

template<> const component_shift ecs::component<curve_walker>::component_bitshift{
		(component_shift)app_component_bits::curve_walker
};

template<> const component_shift ecs::component<control_point>::component_bitshift{
		(component_shift)app_component_bits::control_point
};

curve_walker_loader::curve_walker_loader()
{
}

void curve_walker_loader::load(asset::asset_loader_node& ecs_node)
{
	auto entity = ecs_node.entity_resource.entity;
	auto entity_data = ecs_node.entity_resource.entity_data;
	auto& json = ecs_node.entity_resource.entity_data->component_data(curve_walker::component_bitshift);
	auto& component = entity->get_component<curve_walker>();	
	
	for (auto& c : ecs_node.children)
	{
		auto* e = c.entity_resource.entity;
		if (e->has<control_point>())
		{
			component.control_points.push_back(e);
		}
	}
}

component_bitset curve_walker_loader::components_to_load()
{
	return curve_walker::archetype_bit;
}
