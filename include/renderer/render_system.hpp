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
#include <renderer/render_pass.hpp>
#include <renderer/framebuffer.hpp>
#include <renderer/full_screen_quad.hpp>
#include <renderer/sphere.hpp>
#include <transforms/transform.hpp>
#include <util/string_table.hpp>

namespace renderer
{
	class render_system : public ecs::system_base
	{
		const std::string default_vert = "assets/shaders/default.vert";
		const std::string default_frag = "assets/shaders/default.frag";
		const std::string skybox_vert = "assets/shaders/skybox.vert";
		const std::string skybox_frag = "assets/shaders/skybox.frag";

		const std::string geometry_pass_vert = "assets/shaders/geometry_pass.vert";
		const std::string geometry_pass_frag = "assets/shaders/geometry_pass.frag";
		const std::string lighting_pass_vert = "assets/shaders/lighting_pass.vert";
		const std::string lighting_pass_frag = "assets/shaders/lighting_pass.frag";
		const std::string local_light_pass_vert = "assets/shaders/local_light_pass.vert";
		const std::string local_light_pass_frag = "assets/shaders/local_light_pass.frag";


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
				
		full_screen_quad _fsq;
		sphere _sphere;

		shader_program _default;
		shader_program _skybox;
		shader_program _geometry_pass;
		shader_program _lighting_pass;
		shader_program _local_light_pass;

		framebuffer<3> _gbuffer;		


		std::string shader_src(std::string path);

		void set_light_uniforms(
			ecs::state& state, 
			const renderer::shader_program& shader, 
			renderer::camera& cam);

		void handle_cam_background(const camera& cam);

		void draw_scene(
			ecs::state& state, 
			const shader_program& program);

		void draw_mesh(
			const transforms::transform& transform, 
			const opengl_mesh& mesh,
			const shader_program& program);

		void bind_material(
			const opengl_material& material, 
			const shader_program& program);		
		
		void bind_texture(
			const opengl_texture& texture);

		void bind_camera_uniforms(
			const shader_program& shader, 
			const transforms::transform& transform,
			const camera& cam);

		void draw_skybox(const camera& cam);

		void draw_local_lights(
			ecs::state& state,
			const transforms::transform& camera_transform,
			const camera& cam);
	};
}

#endif