#include <transforms/transform_loader.hpp>

void transforms::transform_loader::load(asset::asset_loader_node& node)
{
    auto entity = node.entity_resource.entity;
    auto entity_data = node.entity_resource.entity_data;
    auto& json = node.entity_resource.entity_data->component_data(transform::component_bitshift);
    auto& component = entity->get_component<transform>();

    auto& position = component.position();
    auto& rotation = component.rotation();
    auto& scale = component.scale();    
    
    if (json.find("position") != json.end())
        position = Eigen::Translation3f(
            json["position"][0].get<float>(),
            json["position"][1].get<float>(),
            json["position"][2].get<float>());

    if (json.find("scale") != json.end())
        for (size_t i = 0; i < 3; ++i)
            scale[i] = json["scale"][i].get<float>();

    load_rotation(json, rotation);

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


void transforms::transform_loader::load_rotation(const json& json, Eigen::Quaternionf& rotation)
{
    if (json.find("rotation") == json.end())
        return;

    Eigen::Vector3f euler_rotation;
    for (size_t i = 0; i < 3; ++i)
        euler_rotation[i] = json["rotation"][i].get<float>();

    rotation = euler_to_quaternion(euler_rotation);
}

Eigen::Quaternionf transforms::transform_loader::euler_to_quaternion(const Eigen::Vector3f euler) const
{
    return Eigen::AngleAxisf(euler[0], Eigen::Vector3f::UnitX())
        * Eigen::AngleAxisf(euler[1], Eigen::Vector3f::UnitY())
        * Eigen::AngleAxisf(euler[2], Eigen::Vector3f::UnitZ());
}