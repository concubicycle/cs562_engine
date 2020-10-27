#include <ecs/system_base.hpp>
#include <core/frame_timer.hpp>

class curve_walker_system : public ecs::system_base
{
	const float Epsilon = 0.1f;

public:
	curve_walker_system(core::frame_timer& time);

	void initialize(ecs::state& state) override;
	void update(ecs::state& state) override;

private:
	core::frame_timer& _time;
};