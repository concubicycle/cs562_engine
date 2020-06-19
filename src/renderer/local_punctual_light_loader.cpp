#include <string>

#include <renderer/local_punctual_light_loader.hpp>
#include <renderer/local_punctual_light.hpp>

#include <Eigen/Core>


void renderer::local_punctual_light_loader::load(asset::asset_loader_node& node)
{
    auto entity = node.entity_resource.entity;
    auto entity_data = node.entity_resource.entity_data;
    auto& json = node.entity_resource.entity_data->component_data(local_punctual_light::component_bitshift);
    auto& component = entity->get_component<local_punctual_light>();

    if (json.find("color") != json.end())
    {
        component.color[0] = json["color"][0].get<float>();
        component.color[1] = json["color"][1].get<float>();
        component.color[2] = json["color"][2].get<float>();
    }

    component.set_radius(json.value("radius", 10.f));
}

component_bitset renderer::local_punctual_light_loader::components_to_load()
{
    return local_punctual_light::archetype_bit;
}