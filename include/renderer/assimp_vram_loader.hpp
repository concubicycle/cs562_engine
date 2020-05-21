#ifndef __ASSIMP_DATA_LOADER_HPP_
#define __ASSIMP_DATA_LOADER_HPP_

#include <cstddef>
#include <nlohmann/json.hpp>
#include <assimp/scene.h>
#include <glbinding/gl/gl.h>

#include <asset/asset_loader.hpp>
#include <asset/assimp_scene_asset.hpp>
#include <asset/assimp_mesh.hpp>
#include <asset/assimp_material.hpp>
#include <renderer/opengl_model.hpp>
#include <renderer/opengl_mesh.hpp>
#include <renderer/opengl_material.hpp>


namespace renderer
{
	class assimp_vram_loader
	{
	public:

		assimp_vram_loader(asset::asset_loader& loader);
		
		/**
			Load model with assimp materials
		*/
		opengl_model load_model(const asset::assimp_scene_asset& scene);

		/**
			Load model with custom materials
		*/
		opengl_model load_model(const asset::assimp_scene_asset& scene, const nlohmann::json& mesh_material_array);

		opengl_material load_material(const nlohmann::json& json, size_t index);
		opengl_material load_material(const asset::assimp_material& material);
		opengl_mesh load_mesh(const asset::assimp_mesh& mesh);
		gl::GLuint load_texture(const asset::texture_asset& texture_asset);

	private:
		asset::asset_loader& _loader;

	};
}

#endif