#include <asset/asset_loader.hpp>
#include <asset/assimp_scene_asset.hpp>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <filesystem>
#include <fstream>

asset::asset_loader::asset_loader()
{
}

asset::asset_loader::~asset_loader()
{
}



nlohmann::json& asset::asset_loader::get_json(const std::string& path)
{	
	auto cached = _json_cache.find(path);
	if (cached != _json_cache.end())
		return cached->second;

	validate_path(path);

	std::ifstream i(path);
	nlohmann::json j;
	i >> j;

	_json_cache.insert(std::make_pair(path, j));
	return _json_cache.find(path)->second;
}

const asset::assimp_scene_asset& asset::asset_loader::get_assimp_scene(const std::string& filepath)
{
	auto cached = _assimp_cache.find(filepath);
	if (cached != _assimp_cache.end())
		return cached->second;

	validate_path(filepath);

	auto& ret = _assimp_cache[filepath];

	const aiScene* scene = ret.importer.ReadFile(filepath,
		aiProcess_ValidateDataStructure |
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_CalcTangentSpace);

	if (!scene || !scene->HasMeshes() || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::string err = "Error: Assimp failed to load mesh from file. \n";
		err += filepath + "\n";
		err += ret.importer.GetErrorString();
		throw std::runtime_error(err);
	}

	ret.aiscene = scene;
	return ret;
}

void asset::asset_loader::validate_path(const std::string& path)
{
	if (!std::filesystem::exists(path))
	{		
		throw std::runtime_error("Invalid path: " + path);
	}
}



