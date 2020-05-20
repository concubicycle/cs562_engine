// Index header, which includes common engine headers for convenience. 

// Engine Systems
#include <asset/asset_manager.hpp>
#include <asset/scene_hydrater.hpp>
#include <asset/scene.hpp>
#include <asset/scene_tracker.hpp>
#include <audio/audio_system.hpp>
#include <audio/music_player.hpp>
#include <audio/loader_emitter.hpp>
#include <audio/loader_music_player.hpp>
#include <collisions/aabb_collider_loader.hpp>
#include <collisions/collider.hpp>
#include <collisions/collision_manager.hpp>
#include <collisions/rigid_body_loader.hpp>
#include <collisions/sphere_collider_loader.hpp>
#include <core/glfw_context.hpp>
#include <core/viewport.hpp>
#include <ecs/register_component.hpp>
#include <ecs/state.hpp>
#include <ecs/world.hpp>
#include <physics/physics_update.hpp>
#include <rendering/animator.hpp>
#include <rendering/camera_updater.hpp>
#include <rendering/camera.hpp>
#include <rendering/debug_view.hpp>
#include <rendering/light_directional.hpp>
#include <rendering/light_point.hpp>
#include <rendering/loader_camera.hpp>
#include <rendering/loader_light_directional.hpp>
#include <rendering/loader_light_point.hpp>
#include <rendering/loader_rigged_model.hpp>
#include <rendering/loader_model.hpp>
#include <rendering/render_loader.hpp>
#include <rendering/renderable_mesh_static.hpp>
#include <rendering/renderable_model_static.hpp>
#include <rendering/renderable_mesh_rigged.hpp>
#include <rendering/renderer.hpp>
#include <transforms/transform_loader.hpp>
#include <transforms/transform.hpp>
#include <transforms/transformer.hpp>


// Overlays
#include <engine-ui/imgui_overlay.hpp>
#include <engine-ui/developer_console.hpp>
#include <engine-ui/entities_view.hpp>
#include <engine-ui/fps_display.hpp>