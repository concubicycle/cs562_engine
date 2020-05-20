#include <string>

#include <renderer/model_loader.hpp>
#include <renderer/model_instance.hpp>



renderer::model_loader::model_loader(util::string_table& strings, asset::asset_loader& loader) 
	: _strings(strings), _asset_loader(loader)
{
}

void renderer::model_loader::load(asset::asset_loader_node& node)
{
    auto entity = node.entity_resource.entity;
    auto entity_data = node.entity_resource.entity_data;
    auto& json = node.entity_resource.entity_data->component_data(model_instance::component_bitshift);
    auto& component = entity->get_component<model_instance>();

    auto model_file = json["model"].get<std::string>();
    auto& scene_asset = _asset_loader.get_assimp_scene(model_file);

    // turn scene_asset into opengl_model

    // if resource has material specified, use it. 
    // otherwise, get it from assimp.
}

component_bitset renderer::model_loader::components_to_load()
{
    return model_instance::archetype_bit;
}
