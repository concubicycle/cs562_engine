#include <asset/asset_loader.hpp>
#include <asset/scene_hydrater.hpp>
#include <asset/scene_tracker.hpp>
#include <core/cursor_state.hpp>
#include <core/frame_limiter.hpp>
#include <core/frame_timer.hpp>
#include <core/glfw_context.hpp>
#include <core/input_manager.hpp>
#include <core/startup_config.hpp>
#include <ecs/archetype_pools.hpp>
#include <ecs/register_component.hpp>
#include <ecs/state.hpp>
#include <ecs/world.hpp>
#include <platform/sleeper.hpp>
#include <util/string_table.hpp>

#include <transforms/transform.hpp>
#include <transforms/transform_loader.hpp>
#include <transforms/transform_system.hpp>

#include <renderer/ambient_light.hpp>
#include <renderer/ambient_light_loader.hpp>
#include <renderer/render_system.hpp>
#include <renderer/model_instance.hpp>
#include <renderer/model_loader.hpp>
#include <renderer/assimp_vram_loader.hpp>
#include <renderer/punctual_light.hpp>
#include <renderer/camera.hpp>
#include <renderer/camera_loader.hpp>
#include <renderer/punctual_light_loader.hpp>
#include <renderer/camera_update_system.hpp>

#include "app_event.hpp"



void run_app();