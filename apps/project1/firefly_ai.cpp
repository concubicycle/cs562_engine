#include "firefly_ai.hpp"

#include <ecs/ecs_types.hpp>
#include <ecs/component.hpp>

#include <transforms/transform.hpp>

#include "app_component_bits.hpp"

#include <random>

#include <Eigen/Geometry>


template<> const component_shift ecs::component<firefly>::component_bitshift {
		(component_shift)app_component_bits::firefly
};


firefly_ai::firefly_ai(core::frame_timer& time)
	: _time(time)
{
}

void firefly_ai::initialize(ecs::state& state)
{
	using namespace transforms;

	state.each<transform, firefly>([&](transform& t, firefly& f) {
		f.target = t.world_position();
		f.center = t.world_position();
	});
}

void firefly_ai::update(ecs::state& state)
{
	using namespace transforms;

	state.each<transform, firefly>([&](transform& t, firefly& f) {
		Eigen::Vector3f to_target = f.target - t.world_position();
		Eigen::Vector3f to_target_norm = to_target.normalized();
		auto& translation = t.position();
		Eigen::Vector3f displacement = to_target.normalized() * f.speed * _time.smoothed_delta_seconds_f();
		translation = translation * Eigen::Translation3f(displacement);

		if (to_target.squaredNorm() < Epsilon)
		{
			std::random_device rd;  //Will be used to obtain a seed for the random number engine
			std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
			std::uniform_real_distribution<float> range(-f.radius, f.radius);
			f.target = f.center + Eigen::Vector3f(range(gen), range(gen), range(gen));
		}
	});
}

