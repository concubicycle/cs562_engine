#include "run_app.hpp"
#include <random>

void generate_lights(asset::scene_hydrater& hydrater);


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
	ecs::register_component<transforms::freefly_component>("freefly_component");
	ecs::register_component<renderer::model_instance>("model_instance");
	ecs::register_component<renderer::punctual_light>("punctual_light");
	ecs::register_component<renderer::camera>("camera");
	ecs::register_component<renderer::ambient_light>("ambient_light");
	ecs::register_component<renderer::directional_light>("directional_light");
	ecs::register_component<renderer::local_punctual_light>("local_punctual_light");
	ecs::register_component<renderer::participating_medium, 2>("participating_medium");
	ecs::register_component<firefly>("firefly");
	

	asset::scene_tracker scene_tracker("assets/scenes/scene.json");
	asset::asset_loader loader;
	asset::scene_hydrater hydrater(state);

	renderer::assimp_vram_loader vram_loader(loader);

	// register systems
	transforms::transform_system transform_system;
	transforms::freefly_system freefly(input, timer);
	renderer::render_system render_system(strings, loader, vram_loader, glfw, config);
	renderer::camera_update_system camera_updater(glfw);
	firefly_ai firefly_system(timer);

	ecs::systems systems({
		&freefly,
		&transform_system,		
		&camera_updater,
		&render_system
	});

	ecs::world world(systems, state);

	// register loaders
	transforms::transform_loader transform_loader;
	renderer::model_loader model_loader(strings, loader, vram_loader);
	renderer::punctual_light_loader punctual_light_loader;
	renderer::local_punctual_light_loader local_punctual_light_loader;
	renderer::directional_light_loader directional_light_loader;
	renderer::camera_loader camera_loader(loader, vram_loader);
	renderer::ambient_light_loader ambient_light_loader;

	hydrater.register_loaders(
		&transform_loader,
		&model_loader,
		&punctual_light_loader,
		&local_punctual_light_loader,
		&directional_light_loader,
		&camera_loader, 
		&ambient_light_loader);
	
	engineui::imgui_overlay overlay(glfw.window(), input, cursor);
	engineui::fps_display fps(glfw, timer);
	engineui::participating_medium_display pm_display(glfw, state);

	overlay.register_views(		
		&fps,
		&pm_display
	);


	while (scene_tracker.has_next() && !glfwWindowShouldClose(glfw.window()))
	{
		asset::scene scene(scene_tracker.next(), loader);
		hydrater.populate_entities(scene);
		hydrater.load();
		world.initialize();

		//generate_lights(hydrater);

		//game loop
		while (!scene_tracker.has_next() && !glfwWindowShouldClose(glfw.window())) {
			timer.start();
			glfwPollEvents();

			input.update();
			world.update();

			overlay.update();

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

void generate_lights(asset::scene_hydrater& hydrater)
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	std::uniform_real_distribution<float> xz_range(-40, 40);
	std::uniform_real_distribution<float> y_range(0, 15);
	std::uniform_real_distribution<float> color(0, 1);
	size_t num_lights = 1000;

	while (num_lights-- > 0)
	{
		auto& e = hydrater.add_from_prototype("assets/prototypes/local_light.json");
		auto& t = e.get_component<transforms::transform>();
		auto& l = e.get_component<renderer::local_punctual_light>();
		l.color = { color(gen), color(gen), color(gen) };
		auto& tref = t.position();
		tref = Eigen::Translation3f(xz_range(gen), y_range(gen), xz_range(gen));
	}
}
