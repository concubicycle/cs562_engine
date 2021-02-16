#include <physics/rigid_body_loader.hpp>
#include <physics/rigid_body.hpp>


void physics::rigid_body_loader::load(asset::asset_loader_node& node)
{
  auto* entity = node.entity_resource.entity;
  const auto* entity_data = node.entity_resource.entity_data;
  auto& json = node.entity_resource.entity_data->component_data(rigid_body::component_bitshift);
  auto& component = entity->get_component<rigid_body>();

  // hard coded cube for now, mesh later
  component.model.emplace({
    point_mass({-1.f, -1.f, -1.f}, 1.f),
    point_mass({1.f, -1.f, -1.f}, 1.f),
    point_mass({1.f, -1.f, 1.f}, 1.f),
    point_mass({-1.f, -1.f, 1.f}, 1.f),

    point_mass({-1.f, 1.f, -1.f}, 1.f),
    point_mass({1.f, 1.f, -1.f}, 1.f),
    point_mass({1.f, 1.f, 1.f}, 1.f),
    point_mass({-1.f, 1.f, 1.f}, 1.f)
  });
}

component_bitset physics::rigid_body_loader::components_to_load()
{
  return rigid_body::archetype_bit;
}
