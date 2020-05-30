#pragma once

#include <ecs/system_base.hpp>
#include <ecs/component.hpp>
#include <core/frame_timer.hpp>
#include <Eigen/Core>

struct firefly : ecs::component<firefly>
{
	Eigen::Vector3f target;
	Eigen::Vector3f center;
	float radius{ 2 };
	float speed{ 4 };
};

class firefly_ai : public ecs::system_base
{
	const float Epsilon = 0.1f;

public:
	firefly_ai(core::frame_timer& time);

	void initialize(ecs::state& state) override;
	void update(ecs::state& state) override;

private:
	core::frame_timer& _time;
};