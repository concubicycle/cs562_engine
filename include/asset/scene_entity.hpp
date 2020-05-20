//
// Created by sava on 1/23/20.
//

#ifndef WIZARDENGINE_SCENE_ENTITY_HPP
#define WIZARDENGINE_SCENE_ENTITY_HPP

#include <vector>
#include <optional>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include "asset/asset_loader.hpp"
#include <ecs/ecs_types.hpp>
#include <asset/resource_id.hpp>


using json = nlohmann::json;

namespace asset
{
    class scene_entity
    {
    public:
        scene_entity(json &entity_json, asset_loader& assets);

        const json &component_data(component_bitset bit) const;

        [[nodiscard]] component_bitset archetype_id() const { return _archetype_id; }

        template <typename TFunc>
        void traverse(TFunc func)
        {
            func(*this);
            for (auto& c : _children) c.traverse(func);
        }

        const std::vector<scene_entity>& children() const { return _children; }

        scene_entity& add_child(json& entity_json, asset_loader& assets);

        bool has_id() const;        
        entity_id id() const;


        json& j() { return _entity_json; }

    private:
        resource_id _id;

        std::map<component_bitset, json> _components;
        std::vector<scene_entity> _children;
        json _entity_json;        
        component_bitset _archetype_id;        


        void merge_component_props(json& target, json& source);

        /**
        * Scene entities may or may not specify a prototype. 
        * We want to standardize the final entity shape, and make it as if it were
        * a prototype. A prototype is of shape { root:  {...}, children: [...] }. 
        * 
        * For an entity with no prototype, the entity's JSON will be the root,
        * and the children will be empty. This will return { root:  {...}, children: [] }
        * 
        * For an entity with a specified prototype, the prototype will be loaded. 
        */
        json inflate_prototype(json& entity_json, asset_loader& assets);

        void build_child_tree(json& children, asset_loader& assets);
    };
}

#endif //WIZARDENGINE_SCENE_ENTITY_HPP
