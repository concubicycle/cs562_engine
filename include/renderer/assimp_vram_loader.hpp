#ifndef __ASSIMP_DATA_LOADER_HPP_
#define __ASSIMP_DATA_LOADER_HPP_

#include <cstddef>
#include <nlohmann/json.hpp>
#include <assimp/scene.h>

#include <asset/assimp_scene_asset.hpp>
#include <renderer/opengl_model.hpp>
#include <renderer/opengl_mesh.hpp>
#include <renderer/opengl_material.hpp>


namespace renderer
{
	class assimp_vram_loader
	{
	public:
		
		/**
			Load model with assimp materials
		*/
		opengl_model load_model(const asset::assimp_scene_asset& scene);

		/**
			Load model with custom materials
		*/
		opengl_model load_model(const asset::assimp_scene_asset& scene, const nlohmann::json& mesh_material_array);

		opengl_material load_material(const nlohmann::json& json, size_t index);
		opengl_material load_material(const aiMesh* mesh);
	};
}

#endif