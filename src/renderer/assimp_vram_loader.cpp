#include <vector>

#include <renderer/assimp_vram_loader.hpp>
#include <asset/assimp_vertex.hpp>

renderer::opengl_model renderer::assimp_vram_loader::load_model(const asset::assimp_scene_asset& scene)
{
	std::vector<asset::assimp_vertex> vertices;
	std::vector<uint32_t> indices;

	return renderer::opengl_model();
}

renderer::opengl_material renderer::assimp_vram_loader::load_material(const nlohmann::json& json)
{
	return opengl_material();
}

renderer::opengl_material renderer::assimp_vram_loader::load_material(const asset::assimp_scene_asset& scene)
{
	return opengl_material();
}