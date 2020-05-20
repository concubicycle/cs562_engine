//
// Created by sava on 1/23/20.
//

#include <unordered_set>

#include <asset/scene_entity.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/component_meta.hpp>



asset::scene_entity::scene_entity(json& entity_json, asset_loader& assets)
    : _id(asset::next_resource_id())
    , _entity_json(entity_json)
    , _archetype_id(0)
{
    std::string str0 = entity_json.dump();

    auto prototype = inflate_prototype(entity_json, assets);    

    std::string str = prototype.dump();

    for (auto& c : prototype["root"]["components"])
    {
        auto type = c["type"].get<std::string>();
        auto shift = ecs::component_meta::type_to_bit[type];
        _archetype_id |= component_bitset(1) << shift;
        _components.insert(std::make_pair(shift, c));
    }

    for (size_t i = 0; i < prototype["children"].size(); ++i)
        prototype["children"][i]["_index_in_prototype"] = i;

    build_child_tree(prototype["children"], assets);

    _children.reserve(512);
}

void asset::scene_entity::build_child_tree(json& children, asset_loader& assets)
{
    std::vector<json> descendant_children;
    std::vector<scene_entity*> inserted_children;

    _children.reserve(children.size());
    inserted_children.reserve(children.size());

    for (auto& c : children)
    {
        if (c.find("parent_index") == c.end()) {
            auto& inserted = _children.emplace_back(c, assets);
            inserted_children.push_back(&inserted);
        }
        else {
            descendant_children.push_back(c);
        }
    }

    while (!descendant_children.empty())
    {
        auto no_change = true;

        for (auto* potential_parent : inserted_children)
        {
            auto parent_index = potential_parent->_entity_json["_index_in_prototype"].get<std::uint32_t>();

            for (auto& descendant : descendant_children)
            {
                auto descendant_parent_index = descendant["parent_index"].get<std::uint32_t>();
                if (descendant_parent_index == parent_index)
                {
                    auto& inserted = potential_parent->_children.emplace_back(descendant, assets);
                    inserted_children.push_back(&inserted);
                    no_change = false;
                }
            }

            auto removal = std::remove_if(descendant_children.begin(), descendant_children.end(),
                [parent_index](json& j) { return j["parent_index"].get<std::uint32_t>() == parent_index; });
            descendant_children.erase(removal, descendant_children.end());
        }

        if (no_change) break;
    }
}

const json& asset::scene_entity::component_data(component_bitset bit) const
{
    return _components.find(bit)->second;
}

json asset::scene_entity::inflate_prototype(json& entity_json, asset_loader& assets)
{
    auto prototype_path_it = entity_json.find("prototype");

    if (prototype_path_it == entity_json.end() && entity_json.find("root") != entity_json.end())
        return entity_json;
    else if (prototype_path_it == entity_json.end())
        return { {"root", entity_json}, {"children", json::array()} };

    auto prototype_path = prototype_path_it.value().get<std::string>();
    json prototype_json = assets.get_json(prototype_path);
    merge_component_props(prototype_json["root"], entity_json);
    return prototype_json;
}

void asset::scene_entity::merge_component_props(json& target, json& source)
{
    std::unordered_set<std::uint8_t> target_component_shifts;
    std::unordered_map<std::uint8_t, json> shift_to_source_component;

    auto& source_components = source["components"];
    auto& target_components = target["components"];

    // keep track of scene entity components.
    for (auto& c : source_components)
    {
        auto type = c["type"].get<std::string>();
        auto shift = ecs::component_meta::type_to_bit[type];
        shift_to_source_component[shift] = c;
    }

    // override prototype component props with scene entity component props
    for (auto& target_component : target_components)
    {
        auto type = target_component["type"].get<std::string>();
        auto shift = ecs::component_meta::type_to_bit[type];
        target_component_shifts.insert(shift);

        auto it = shift_to_source_component.find(shift);
        if (it == shift_to_source_component.end()) continue;

        auto& entity_component = it->second;
        for (auto& el : entity_component.items())
        {
            target_component[el.key()] = el.value();
        }
    }

    // insert components that are in scene, but not prototype
    for (auto& c : source_components)
    {
        auto type = c["type"].get<std::string>();
        auto shift = ecs::component_meta::type_to_bit[type];
        if (target_component_shifts.find(shift) == target_component_shifts.end())
        {
            target_components.push_back(c);
        }
    }
}

asset::scene_entity& asset::scene_entity::add_child(json& entity_json, asset_loader& assets)
{
    return _children.emplace_back(entity_json, assets);
}

bool asset::scene_entity::has_id() const
{
    return _entity_json.find("entity_id") != _entity_json.end();
}

entity_id asset::scene_entity::id() const
{
    return _entity_json["entity_id"].get<entity_id>();
}
