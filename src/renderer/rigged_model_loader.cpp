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
  auto& transform = entity->get_component<transforms::transform>();

  component.animation_structures.emplace(proto_model.aiscene);
  component.ik_solver.emplace("Bone.008", &(component.animation_structures->skeleton), &transform);
  component.ik_solver->set_target({ 3, 3 ,3 });

  component.is_paused = false;
  component.model = _vram_loader.load_rigged_model(proto_model);
  component.model_filename_hash = _strings.hash_and_store(path);
  component.aiscene = proto_model.aiscene;

  if (component.has_animations())
    component.set_animation_index(0);


  for (size_t i = 0; i < component.animation_structures->skeleton.joints.size(); ++i)
  {
    auto joint = component.animation_structures->skeleton.joints[i];
    std::cout << "Bind Pose" << std::endl;
    std::cout << joint.bind_pose;
    std::cout << std::endl;

    std::cout << "Bind Pose Global Model Space" << std::endl;
    std::cout << component.animation_structures->bind_pose.global_model_space_poses[i];
    std::cout << std::endl;

    std::cout << "Bind Pose Global Bone Space" << std::endl;
    std::cout << component.animation_structures->bind_pose.global_bone_space_poses[i];
    std::cout << std::endl;

    std::cout << "Offset" << std::endl;
    std::cout << joint.mesh_to_bone_space;
    std::cout << std::endl;

    if (component.ik_solver)
    {
      std::cout << "IK Pose" << std::endl;
      std::cout << component.ik_solver->result_pose().global_model_space_poses[i];
      std::cout << std::endl;
    }

    std::cout << "=================" << std::endl;
  }
}

component_bitset renderer::rigged_model_loader::components_to_load()
{
  return rigged_model_instance::archetype_bit;
}