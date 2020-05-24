#include <string>

#include <renderer/camera.hpp>
#include <renderer/camera_loader.hpp>
#include <eigen/Core>

renderer::camera_loader::camera_loader(
    asset::asset_loader& loader, 
    assimp_vram_loader& vram_loader)
    : _loader(loader)
    , _vram_loader(vram_loader)
{
}

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

    if (json.find("skybox") != json.end())
    {
        auto left_path = json["skybox"]["left"].get<std::string>();
        auto top_path = json["skybox"]["top"].get<std::string>();
        auto front_path = json["skybox"]["front"].get<std::string>();
        auto bottom_path = json["skybox"]["bottom"].get<std::string>();
        auto right_path = json["skybox"]["right"].get<std::string>();
        auto back_path = json["skybox"]["back"].get<std::string>();

        auto& left_texture = _loader.get_texture(left_path);
        auto& top_texture = _loader.get_texture(top_path);
        auto& front_texture = _loader.get_texture(front_path);
        auto& bottom_texture = _loader.get_texture(bottom_path);
        auto& right_texture = _loader.get_texture(right_path);
        auto& back_texture = _loader.get_texture(back_path);

        component.skybox_cubemap = _vram_loader.load_cubemap(
            left_texture,
            top_texture,
            front_texture,
            bottom_texture,
            right_texture,
            back_texture
        );
    }
}

component_bitset renderer::camera_loader::components_to_load()
{
    return camera::archetype_bit;
}


