#include <renderer/directional_light_loader.hpp>
#include <renderer/directional_light.hpp>
#include <string>
#include <Eigen/Core>


void renderer::directional_light_loader::load(asset::asset_loader_node& node)
{
    auto entity = node.entity_resource.entity;
    auto entity_data = node.entity_resource.entity_data;
    auto& json = node.entity_resource.entity_data->component_data(directional_light::component_bitshift);
    auto& component = entity->get_component<directional_light>();

    if (json.find("color") != json.end())
    {
        component.color[0] = json["color"][0].get<float>();
        component.color[1] = json["color"][1].get<float>();
        component.color[2] = json["color"][2].get<float>();
    }

    component.direction[0] = json["direction"][0].get<float>();
    component.direction[1] = json["direction"][1].get<float>();
    component.direction[2] = json["direction"][2].get<float>();
}

component_bitset renderer::directional_light_loader::components_to_load()
{
    return directional_light::archetype_bit;
}
