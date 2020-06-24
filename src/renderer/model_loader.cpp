#include <string>

#include <renderer/model_loader.hpp>
#include <renderer/model_instance.hpp>



renderer::model_loader::model_loader(
    util::string_table& strings, 
    asset::asset_loader& loader,
    assimp_vram_loader& vram_loader)
	: _strings(strings)
    , _asset_loader(loader)
    , _vram_loader(vram_loader)
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
    component.model = _vram_loader.load_model(scene_asset);
    component.model_file_hash = _strings.hash_and_store(model_file);
    component.is_closed_shape = json.value("is_closed_shape", true);

    if (json.find("materials") != json.end())
    {
        auto& mesh_definition_indices = json["materials"]["mesh_definition_indices"];
        auto& definitions = json["materials"]["definitions"];

        for (size_t i = 0; i < component.model.mesh_count; ++i)
        {
            size_t index = i >= mesh_definition_indices.size()
                ? mesh_definition_indices.size() - 1
                : mesh_definition_indices[i].get<size_t>();

            assign_material(component.model.meshes[i].material, definitions[index]);
        }
    }
}

component_bitset renderer::model_loader::components_to_load()
{
    return model_instance::archetype_bit;
}


void renderer::model_loader::assign_material(opengl_material& material, json definition)
{
    material.roughness = definition.value("roughness", 1.0f);

    material.fresnel_color = Eigen::Array3f(
        definition["fresnel_color"][0].get<float>(),
        definition["fresnel_color"][1].get<float>(),
        definition["fresnel_color"][2].get<float>());
}