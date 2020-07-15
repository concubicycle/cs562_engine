#ifndef __RENDER_SYSTEM_HPP_
#define __RENDER_SYSTEM_HPP_

#include <asset/asset_loader.hpp>
#include <core/glfw_context.hpp>
#include <core/startup_config.hpp>
#include <ecs/system_base.hpp>
#include <renderer/assimp_vram_loader.hpp>
#include <renderer/camera.hpp>
#include <renderer/compute_shader_prograam.hpp>
#include <renderer/hammersley.hpp>
#include <renderer/framebuffer.hpp>
#include <renderer/full_screen_quad.hpp>
#include <renderer/model_instance.hpp>
#include <renderer/opengl_context.hpp>
#include <renderer/opengl_material.hpp>
#include <renderer/opengl_mesh.hpp>
#include <renderer/render_pass.hpp>
#include <renderer/shader_program.hpp>
#include <renderer/sphere.hpp>
#include <renderer/punctual_light.hpp>
#include <renderer/directional_light.hpp>
#include <transforms/transform.hpp>
#include <util/string_table.hpp>


namespace renderer
{
	class render_system : public ecs::system_base
	{
	public:
		render_system(
			util::string_table& strings,
			asset::asset_loader& assets,
			assimp_vram_loader& vram_loader,
			core::glfw_context& glfw,
			core::startup_config& config);

		virtual void initialize(ecs::state& state) override;
		virtual void update(ecs::state& state) override;

	private:
		opengl_context _opengl;

		util::string_table& _strings;
		asset::asset_loader& _assets;
		assimp_vram_loader& _vram_loader;
		core::glfw_context& _glfw;
		core::startup_config& _config;
		full_screen_quad _fsq;

		sphere _sphere{4, 4};
		sphere skydome_mesh{ 64, 128};
		opengl_hammersley_block<40> _hammersley_block{};

		shader_program _default{ 
			_assets.get_text("assets/shaders/default.vert"),
			_assets.get_text("assets/shaders/default.frag") };
		
		shader_program _skybox{ 
			_assets.get_text("assets/shaders/skybox.vert"),
			_assets.get_text("assets/shaders/skybox.frag") };

		shader_program _skydome{
			_assets.get_text("assets/shaders/skydome.vert"),
			_assets.get_text("assets/shaders/skydome.frag") };
		
		shader_program _geometry_pass{
			_assets.get_text("assets/shaders/geometry_pass.vert"),
			_assets.get_text("assets/shaders/geometry_pass.frag") };
		
		shader_program _lighting_pass{ 
			_assets.get_text("assets/shaders/lighting_pass.vert"),
			_assets.get_text("assets/shaders/lighting_pass.frag") };

		shader_program _local_light_pass{ 
			_assets.get_text("assets/shaders/local_light_pass.vert"),
			_assets.get_text("assets/shaders/local_light_pass.frag") };

		shader_program _dual_paraboloid_shadow{ 
			_assets.get_text("assets/shaders/dual_paraboloid_shadow.vert"),
			_assets.get_text("assets/shaders/dual_paraboloid_shadow.frag") };

		shader_program _directional_shadow{
			_assets.get_text("assets/shaders/directional_shadow.vert"),
			_assets.get_text("assets/shaders/directional_shadow.frag") };

		compute_shader_program _gaussian_horizontal{ 
			_assets.get_text("assets/shaders/gaussian_horizontal.comp") };

		compute_shader_program _gaussian_vertical{ 
			_assets.get_text("assets/shaders/gaussian_vertical.comp") };
		
		framebuffer<4> _gbuffer{
		    (float)_glfw.width(),
		    (float)_glfw.height(),
			texture_description(
				gl::GLenum::GL_COLOR_ATTACHMENT0,
				_glfw.width(),
				_glfw.height(),
				gl::GLenum::GL_RGBA32F,
				gl::GLenum::GL_RGBA,
				gl::GLenum::GL_FLOAT),
			texture_description(
				gl::GLenum::GL_COLOR_ATTACHMENT1,
				_glfw.width(),
				_glfw.height(),
				gl::GLenum::GL_RGBA32F,
				gl::GLenum::GL_RGBA,
				gl::GLenum::GL_FLOAT),
			texture_description(
				gl::GLenum::GL_COLOR_ATTACHMENT2,
				_glfw.width(),
				_glfw.height(),
				gl::GLenum::GL_RGBA,
				gl::GLenum::GL_RGBA,
				gl::GLenum::GL_FLOAT),
			texture_description(
				gl::GLenum::GL_COLOR_ATTACHMENT3,
				_glfw.width(),
				_glfw.height(),
				gl::GLenum::GL_RGBA,
				gl::GLenum::GL_RGBA,
				gl::GLenum::GL_FLOAT) };

		void set_light_uniforms(
			ecs::state& state, 
			const renderer::shader_program& shader, 
			renderer::camera& cam);

		void handle_cam_background(const camera& cam);

		void draw_scene(
			ecs::state& state, 
			const shader_program& program);

		void draw_scene_shadowmap(ecs::state& state);

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
		void draw_skydome(const camera& cam);

		void draw_local_lights(
			ecs::state& state,
			const transforms::transform& camera_transform,
			const camera& cam);

		void render_shadowmap(
			ecs::state& state, 
			transforms::transform& t, 
			punctual_light& pl);

		void render_shadowmap_directional(
			ecs::state& state,
			transforms::transform& t,
			directional_light& dl);

		opengl_mesh load_icosphere();
	};
}

#endif
