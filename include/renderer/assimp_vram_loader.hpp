#ifndef __ASSIMP_DATA_LOADER_HPP_
#define __ASSIMP_DATA_LOADER_HPP_

#include <nlohmann/json.hpp>

#include <asset/assimp_scene_asset.hpp>
#include <renderer/opengl_model.hpp>
#include <renderer/opengl_material.hpp>


namespace renderer
{
	class assimp_vram_loader
	{
	public:
		
		opengl_model load_model(const asset::assimp_scene_asset& scene);
		opengl_material load_material(const nlohmann::json& json);
		opengl_material load_material(const asset::assimp_scene_asset& scene);
	};
}

#endif