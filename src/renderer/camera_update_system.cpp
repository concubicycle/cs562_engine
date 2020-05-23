#include <renderer/camera_update_system.hpp>
#include <renderer/projections.hpp>
#include <renderer/camera.hpp>
#include <transforms/transform.hpp>


renderer::camera_update_system::camera_update_system(core::glfw_context& glfw)
	: _glfw(glfw)
{
}

void renderer::camera_update_system::initialize(ecs::state& state)
{
	using namespace transforms;

	state.each<transform, camera>([&](transform& t, camera& c) {
		auto aspect = (float)_glfw.width() / (float)_glfw.height();
		c.projection = perspective(c.fov, aspect, c.near_distance, c.far_distance);
	});
}

void renderer::camera_update_system::update(ecs::state& state)
{
	using namespace transforms;

	state.each<transform, camera>([&](transform& t, camera& c) {
		auto& rotation = t.rotation();
		auto& translation = t.position();
		c.inverse_view = translation * rotation;
		c.view = c.inverse_view.inverse();
	});
}
