#include <string>

#include <renderer/camera.hpp>
#include <renderer/camera_loader.hpp>
#include <eigen/Core>


void renderer::camera_loader::load(asset::asset_loader_node& node)
{
    auto entity = node.entity_resource.entity;
    auto entity_data = node.entity_resource.entity_data;
    auto& json = node.entity_resource.entity_data->component_data(camera::component_bitshift);
    auto& component = entity->get_component<camera>();

    component.near_distance = json.value("near_distance", component.near_distance);
    component.far_distance = json.value("far_distance", component.far_distance);
    component.fov = json.value("fov", component.fov);
    component.background = json.value("background", component.background);

    if (json.find("clear_color") != json.end())
    {
        component.clear_color[0] = json["clear_color"][0].get<float>();
        component.clear_color[1] = json["clear_color"][1].get<float>();
        component.clear_color[2] = json["clear_color"][2].get<float>();
        component.clear_color[3] = json["clear_color"][3].get<float>();
    }
}

component_bitset renderer::camera_loader::components_to_load()
{
    return camera::archetype_bit;
}