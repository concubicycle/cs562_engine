#include <asset/scene_hydrater.hpp>
#include <iostream>

asset::scene_hydrater::scene_hydrater(ecs::state &ecs_state) : _ecs_state(ecs_state)
{
   
}

void asset::scene_hydrater::hydrate_recurse(asset_loader_node &graph_entity)
{
    for (auto &c : graph_entity.entity_resource.entity_data->children())
    {
        auto &ecs_entity = c.has_id()
                           ? _ecs_state.add_entity(c.archetype_id(), c.id())
                           : _ecs_state.add_entity(c.archetype_id());

        auto &graph_child = graph_entity.children.emplace_back(ecs_entity, c);
        hydrate_recurse(graph_child);
    }
}


void asset::scene_hydrater::load_recurse(asset_loader_node &entity)
{
    // depth first
    for (auto& child : entity.children)
        load_recurse(child);

    for (auto *loader : _component_loaders)
    {
        auto arch = loader->components_to_load();
        auto &entity_r = entity.entity_resource.entity_data;
        auto entity_arch = entity.entity_resource.entity->archetype_id();

        if ((arch & entity_arch) == arch)
            loader->load(entity);
    }
}

ecs::entity &asset::scene_hydrater::add_from_prototype(const std::string &path)
{
    auto &scene_entity = _scene->add_from_prototype(path);
    auto &ecs_entity = _ecs_state.add_entity(scene_entity.archetype_id());
    auto &graph_entity = _entity_refs.emplace_back(ecs_entity, scene_entity);
    hydrate_recurse(graph_entity);
    load_recurse(graph_entity);
    return ecs_entity;
}

void asset::scene_hydrater::remove_entity(ecs::entity &entity)
{
    auto removal = std::remove_if(
            _entity_refs.begin(),
            _entity_refs.end(),
            [&](const asset_loader_node &n) { return entity.id() == n.entity_resource.entity->id(); });

    _entity_refs.erase(removal, _entity_refs.end());
    _to_remove.push_back(&entity);
}

void asset::scene_hydrater::remove_entity(entity_id id)
{
    auto& e = _ecs_state.find_entity(id);    
    remove_entity(e);
}

void asset::scene_hydrater::flush_removed()
{
    std::sort(_to_remove.begin(), _to_remove.end()); 
    auto last = std::unique(_to_remove.begin(), _to_remove.end());
    _to_remove.erase(last, _to_remove.end());

    for (auto* e : _to_remove)
        _ecs_state.remove_entity(*e);
    
    _to_remove.clear();
}

void asset::scene_hydrater::load()
{
    for (auto& e : _entity_refs)
        load_recurse(e);
}

void asset::scene_hydrater::populate_entities(asset::scene& scene)
{
    _scene = &scene;

    for (auto& scene_entity : scene.entities())
    {
        auto& ecs_entity = scene_entity.has_id()
            ? _ecs_state.add_entity(scene_entity.archetype_id(), scene_entity.id())
            : _ecs_state.add_entity(scene_entity.archetype_id());

        auto& graph_entity = _entity_refs.emplace_back(ecs_entity, scene_entity);
        hydrate_recurse(graph_entity);
    }
}

void asset::scene_hydrater::clear()
{
    _entity_refs.clear();    
    _to_remove.clear();
}