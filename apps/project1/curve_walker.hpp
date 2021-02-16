#pragma once

#include <optional>
#include <vector>
#include <ecs/component.hpp>
#include <ecs/entity.hpp>
#include <Eigen/Core>
#include <renderer/skeletal_animation_frame.hpp>
#include <renderer/animation_time.hpp>
#include <renderer/timeline.hpp>

#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>

#include <math/curve.hpp>

struct control_point : public ecs::component<control_point>
{
  Eigen::Vector3f tangent = Eigen::Vector3f::Zero();
};

struct curve_walker : public ecs::component<curve_walker>
{
  std::vector<ecs::entity*> control_points;
  ecs::entity* walkee;
    
  float s{ 0 }; // curve param
  float r{ 1 }; // speed
  float d{ 0 }; // distance
  float t{ 0 };

  Eigen::Vector2f pos;
  Eigen::Vector2f last_pos;
  float last_d{ 0 };

  float arc_length_tolerance{ 0.01f };

  float t_rampup{ 6.f };
  float t_rampdown{ 6.f };
  float stop_t;
  float end_t;
  bool is_stopping{ false };

  void stop()
  {
    is_stopping = true;
    stop_t = t;
    end_t = t + t_rampdown;
  }

  std::optional<math::cubic_b_spline> curve;
  std::optional<math::arc_length_table> ark_lengths;
};

class curve_walker_loader : public asset::component_loader
{
public:
	curve_walker_loader();

	virtual void load(asset::asset_loader_node& ecs_node) override;
	virtual component_bitset components_to_load() override;

private:
	
};