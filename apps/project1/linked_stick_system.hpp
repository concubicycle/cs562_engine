#pragma once

#include <ecs/system_base.hpp>
#include <ecs/component.hpp>
#include <core/frame_timer.hpp>
#include <core/input_manager.hpp>
#include <Eigen/Core>
#include <ecs/entity.hpp>
#include <asset/scene_hydrater.hpp>

#include <physics/model.hpp>
#include <physics/spring.hpp>

namespace transforms {
struct transform;
}

struct linked_sticks : ecs::component<linked_sticks>
{
  std::string stick_prefab { "assets/prototypes/stick.json" };

  std::uint32_t num_links {5};
  float stick_length {3};  
  Eigen::Vector3f anchor { Eigen::Vector3f::Zero() };

  std::vector<entity_id> stick_markers;
  std::vector<physics::model> stick_models;
  std::vector<physics::spring> springs;
};

class linked_stick_system : public ecs::system_base
{
public:
  explicit linked_stick_system(
    core::frame_timer& timer,
    core::input_manager& input,
    asset::scene_hydrater& hydrater);

  void initialize(ecs::state& state) override;
  void update(ecs::state& state) override;

private:
  core::frame_timer& _timer;
  core::input_manager& _input;
  asset::scene_hydrater& _hydrater;
  

  void add_forces(ecs::state& state, transforms::transform& t, linked_sticks& ls);
  void step_physics(ecs::state& state, transforms::transform& t, linked_sticks& ls);
  void sync_transforms(ecs::state& state, transforms::transform& t, linked_sticks& ls);

  void do_input(ecs::state& state, transforms::transform& t, linked_sticks& ls);
};


