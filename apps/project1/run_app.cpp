#include "run_app.hpp"

void run_app()
{
	app_events events;
	os::sleeper sleeper;
	util::string_table strings;

	core::startup_config config;
	core::glfw_context glfw(config);
	core::input_manager input(glfw.window());
	core::frame_timer timer;
	core::frame_limiter limiter(timer, 60, sleeper);
	core::cursor_state cursor(glfw.window());

	ecs::archetype_pools memory;
	ecs::state state(memory);
	ecs::register_component<transforms::transform>("transform");
	ecs::register_component<renderer::model_instance>("model_instance");
	ecs::register_component<renderer::punctual_light>("punctual_light");
	ecs::register_component<renderer::camera>("camera");

	asset::scene_tracker scene_tracker("assets/scenes/scene.json");
	asset::asset_loader loader;
	asset::scene_hydrater hydrater(state);

	renderer::assimp_vram_loader vram_loader(loader);

	// register systems
	transforms::transform_system transform_system;
	renderer::render_system render_system;

	ecs::systems systems({
		&transform_system,
		&render_system
		});
	ecs::world world(systems, state);

	// register loaders
	transforms::transform_loader transform_loader;
	renderer::model_loader model_loader(strings, loader, vram_loader);
	renderer::punctual_light_loader punctual_light_loader;
	renderer::camera_loader camera_loader;

	hydrater.register_loaders(
		&transform_loader,
		&model_loader,
		&punctual_light_loader,
		&camera_loader);

	while (scene_tracker.has_next() && !glfwWindowShouldClose(glfw.window()))
	{
		asset::scene scene(scene_tracker.next(), loader);
		hydrater.populate_entities(scene);
		hydrater.load();
		world.initialize();

		//game loop
		while (!scene_tracker.has_next() && !glfwWindowShouldClose(glfw.window())) {
			timer.start();
			glfwPollEvents();

			input.update();
			world.update();

			glfw.swap_buffers();
			hydrater.flush_removed();

			limiter.wait_remainder();
			timer.end();
		}

		world.cleanup();
		hydrater.clear();
		state.free_all();
	}
}
