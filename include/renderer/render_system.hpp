#ifndef __RENDER_SYSTEM_HPP_
#define __RENDER_SYSTEM_HPP_

#include <asset/asset_loader.hpp>
#include <core/glfw_context.hpp>
#include <core/startup_config.hpp>
#include <ecs/system_base.hpp>
#include <renderer/assimp_vram_loader.hpp>
#include <renderer/camera.hpp>
#include <renderer/compute_shader_prograam.hpp>
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
		const std::string skydome_vert = "assets/shaders/skydome.vert";
		const std::string skydome_frag = "assets/shaders/skydome.frag";
		const std::string geometry_pass_vert = "assets/shaders/geometry_pass.vert";
		const std::string geometry_pass_frag = "assets/shaders/geometry_pass.frag";
		const std::string lighting_pass_vert = "assets/shaders/lighting_pass.vert";
		const std::string lighting_pass_frag = "assets/shaders/lighting_pass.frag";
		const std::string local_light_pass_vert = "assets/shaders/local_light_pass.vert";
		const std::string local_light_pass_frag = "assets/shaders/local_light_pass.frag";
		const std::string dual_paraboloid_shadow_vert = "assets/shaders/dual_paraboloid_shadow.vert";
		const std::string dual_paraboloid_shadow_frag = "assets/shaders/dual_paraboloid_shadow.frag";
		const std::string gaussian_horizontal_comp = "assets/shaders/gaussian_horizontal.comp";
		const std::string gaussian_vertical_comp = "assets/shaders/gaussian_vertical.comp";

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

		opengl_mesh _icosphere{ load_icosphere() };

		shader_program _default{ 
			_assets.get_text(default_vert), 
			_assets.get_text(default_frag) };
		
		shader_program _skybox{ 
			_assets.get_text(skybox_vert), 
			_assets.get_text(skybox_frag) };

		shader_program _skydome{
			_assets.get_text(skydome_vert),
			_assets.get_text(skydome_frag) };
		
		shader_program _geometry_pass{
			_assets.get_text(geometry_pass_vert), 
			_assets.get_text(geometry_pass_frag) };
		
		shader_program _lighting_pass{ 
			_assets.get_text(lighting_pass_vert), 
			_assets.get_text(lighting_pass_frag) };

		shader_program _local_light_pass{ 
			_assets.get_text(local_light_pass_vert), 
			_assets.get_text(local_light_pass_frag) };

		shader_program _dual_paraboloid_shadow{ 
			_assets.get_text(dual_paraboloid_shadow_vert), 
			_assets.get_text(dual_paraboloid_shadow_frag) };

		compute_shader_program _gaussian_horizontal{ 
			_assets.get_text(gaussian_horizontal_comp) };

		compute_shader_program _gaussian_vertical{ 
			_assets.get_text(gaussian_vertical_comp) };
		
		framebuffer<4> _gbuffer{
			_glfw.width(),
			_glfw.height(),
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

		opengl_mesh load_icosphere();
	};
}

#endif