#include <string>

#include <renderer/ambient_light.hpp>
#include <renderer/ambient_light_loader.hpp>
#include <Eigen/Core>


void renderer::ambient_light_loader::load(asset::asset_loader_node& node)
{
    auto entity = node.entity_resource.entity;
    auto entity_data = node.entity_resource.entity_data;
    auto& json = node.entity_resource.entity_data->component_data(ambient_light::component_bitshift);
    auto& component = entity->get_component<ambient_light>();
    
    component.color[0] = json["color"][0].get<float>();
    component.color[1] = json["color"][1].get<float>();
    component.color[2] = json["color"][2].get<float>();
}

component_bitset renderer::ambient_light_loader::components_to_load()
{
    return ambient_light::archetype_bit;
}
