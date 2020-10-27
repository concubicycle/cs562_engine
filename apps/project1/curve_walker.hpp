#pragma once

#include <vector>
#include <ecs/component.hpp>
#include <ecs/entity.hpp>
#include <Eigen/Core>
#include <renderer/skeletal_animation_frame.hpp>
#include <renderer/animation_time.hpp>
#include <renderer/timeline.hpp>


#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>

struct control_point : public ecs::component<control_point>
{
  Eigen::Vector3f tangent = Eigen::Vector3f::Zero();
};

struct curve_walker : public ecs::component<curve_walker>
{
  std::vector<ecs::entity*> control_points;
  ecs::entity* walkee;

  renderer::animation_time s; // curve parameter
  float r; // speed
  float d; // distance
  float v; // max animation speed (this is the max global playback rate. can be >1)

};

class curve_walker_loader : public asset::component_loader
{
public:
	curve_walker_loader();

	virtual void load(asset::asset_loader_node& ecs_node) override;
	virtual component_bitset components_to_load() override;

private:
	
};