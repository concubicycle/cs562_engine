#include <renderer/rigged_model_loader.hpp>
#include <renderer/rigged_model_instance.hpp>
#include <renderer/skeleton_joint.hpp>
#include <asset/bone_flattener.hpp>
#include <spdlog/spdlog.h>

renderer::rigged_model_loader::rigged_model_loader(
  util::string_table& strings,
  asset::asset_loader& asset_loader,
  assimp_vram_loader& vram_loader)
  : _strings(strings)
  , _asset_loader(asset_loader)
  , _vram_loader(vram_loader)
{
}

void renderer::rigged_model_loader::load(asset::asset_loader_node& ecs_node)
{
  auto entity = ecs_node.entity_resource.entity;
  auto entity_data = ecs_node.entity_resource.entity_data;
  auto& json = entity_data->component_data(renderer::rigged_model_instance::component_bitshift);
  auto& component = entity->get_component<renderer::rigged_model_instance>();
  auto& path = json["model"].get<std::string>();
  auto& proto_model = _asset_loader.get_assimp_scene(path);
    
  component.animation_structures.emplace(proto_model.aiscene);
  component.is_paused = false;
  component.model = _vram_loader.load_rigged_model(proto_model);
  component.model_filename_hash = _strings.hash_and_store(path);
  component.aiscene = proto_model.aiscene;
}

component_bitset renderer::rigged_model_loader::components_to_load()
{
  return rigged_model_instance::archetype_bit;
}