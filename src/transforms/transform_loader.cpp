#include <transforms/transform_loader.hpp>

void transforms::transform_loader::load(asset::asset_loader_node& node)
{
    auto entity = node.entity_resource.entity;
    auto entity_data = node.entity_resource.entity_data;
    auto& json = node.entity_resource.entity_data->component_data(transform::component_bitshift);
    auto& component = entity->get_component<transform>();

    auto& position = component.position();
    auto& rotation = component.rotation_euler();
    auto& scale = component.scale();    
    
    if (json.find("position") != json.end())
        position = Eigen::Translation3f(
            json["position"][0].get<float>(),
            json["position"][1].get<float>(),
            json["position"][2].get<float>());

    if (json.find("scale") != json.end())
        for (size_t i = 0; i < 3; ++i)
            scale[i] = json["scale"][i].get<float>();

    if (json.find("rotation") != json.end())
        for (size_t i = 0; i < 3; ++i)
            rotation[i] = json["rotation"][i].get<float>();

    for (auto& child : node.children)
    {
        auto child_id = child.entity_resource.entity->id();        
        component.add_child(child.entity_resource.entity);

        auto& child_transform = child.entity_resource.entity->get_component<transform>();        
        child_transform.parent() = entity;
    }
}

component_bitset transforms::transform_loader::components_to_load()
{
    return transform::archetype_bit;
}

