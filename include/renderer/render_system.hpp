#ifndef __RENDER_SYSTEM_HPP_
#define __RENDER_SYSTEM_HPP_

#include <asset/asset_loader.hpp>
#include <core/glfw_context.hpp>
#include <core/startup_config.hpp>
#include <ecs/system_base.hpp>
#include <renderer/camera.hpp>
#include <renderer/model_instance.hpp>
#include <renderer/opengl_context.hpp>
#include <renderer/opengl_material.hpp>
#include <renderer/opengl_mesh.hpp>
#include <renderer/shader_program.hpp>
#include <transforms/transform.hpp>
#include <util/string_table.hpp>

namespace renderer
{
	class render_system : public ecs::system_base
	{
		const std::string default_vert = "assets/shaders/default.vert";
		const std::string default_frag = "assets/shaders/default.frag";

	public:
		render_system(
			util::string_table& strings, 
			asset::asset_loader& assets,
			core::glfw_context& glfw,
			core::startup_config& config);

		virtual void update(ecs::state& state) override;

	private:
		opengl_context _opengl;

		util::string_table& _strings;
		asset::asset_loader& _assets;
		core::glfw_context& _glfw;
		core::startup_config& _config;
		
		shader_program _default;

		std::string shader_src(std::string path);

		void set_light_uniforms(
			ecs::state& state, 
			const renderer::shader_program& shader, 
			renderer::camera& cam);

		void handle_cam_background(const shader_program& shader, const camera& cam);

		void draw_scene(ecs::state& state);
		void draw_mesh(const transforms::transform& transform, const opengl_mesh& mesh);

		void bind_material(const opengl_material& material, const shader_program& program);		
		void bind_texture(const opengl_texture& texture);
		void bind_camera_uniforms(
			const shader_program& shader, 
			const transforms::transform& transform,
			const camera& cam);
	};
}

#endif