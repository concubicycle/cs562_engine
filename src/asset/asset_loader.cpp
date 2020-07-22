#include <asset/asset_loader.hpp>
#include <asset/assimp_scene_asset.hpp>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include <filesystem>
#include <fstream>

#include <util/read_file.hpp>




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

	auto& scene_asset = _assimp_cache[filepath];

	const aiScene* scene = scene_asset.importer.ReadFile(filepath,
		aiProcess_ValidateDataStructure |
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_FixInfacingNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
		aiProcess_CalcTangentSpace);

	if (!scene || !scene->HasMeshes() || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
	{
		std::string err = "Error: Assimp failed to load mesh from file. \n";
		err += filepath + "\n";
		err += scene_asset.importer.GetErrorString();
		throw std::runtime_error(err);
	}

	scene_asset.aiscene = scene;
	return scene_asset;
}

const asset::texture_asset& asset::asset_loader::get_texture(const std::string& filepath)
{
	auto cached = _texture_cache.find(filepath);
	if (cached != _texture_cache.end())
		return cached->second;

	validate_path(filepath);
	
	int width, height, channels;
	auto bytes = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
	if (!bytes)
	{
		throw std::runtime_error("Failed to load texture " + filepath);
	}

	_texture_cache.try_emplace(
		filepath, 
		width, height, channels, bytes);

	return _texture_cache.find(filepath)->second;
}

const asset::texture_assetf& asset::asset_loader::get_texturef(
	const std::string& file)
{
	auto cached = _hdr_texture_cache.find(file);
	if (cached != _hdr_texture_cache.end())
		return cached->second;

	validate_path(file);

	int width, height, channels;
	auto floats = stbi_loadf(file.c_str(), &width, &height, &channels, 0);
	if (!floats)
	{
		throw std::runtime_error("Failed to load hdr texture " + file);
	}
	
	// Gamma correct the image to linear color space.  Use gamma=2.2
	// if you have no specific gamma information.
	#pragma omp parallel for schedule(dynamic, 1) // Magic: Multi-thread y loop
	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			int p = (j * width + i);
			for (int c = 0; c < 3; c++) {
				floats[3 * p + c] *= pow(floats[3 * p + c], 1.8);
			}
		}
	}	

	_hdr_texture_cache.try_emplace(
		file,
		width, height, channels, floats);

	return _hdr_texture_cache.find(file)->second;
}

#include <iostream>
#include <iomanip>

const asset::texture_assetf& asset::asset_loader::get_texturef_untonemapped(const std::string& file)
{
	auto cached = _hdr_texture_cache.find(file);
	if (cached != _hdr_texture_cache.end())
		return cached->second;

	validate_path(file);

	int width, height, channels;
	auto floats = stbi_loadf(file.c_str(), &width, &height, &channels, 0);
	if (!floats)
	{
		throw std::runtime_error("Failed to load hdr texture " + file);
	}

	float max = 0;

	_hdr_texture_cache.try_emplace(
		file,
		width, height, channels, floats);

	return _hdr_texture_cache.find(file)->second;
}

const std::string& asset::asset_loader::get_text(const std::string& filepath)
{
	auto cached = _text_cache.find(filepath);
	if (cached != _text_cache.end())
		return cached->second;

	validate_path(filepath);

	_text_cache.try_emplace(filepath, util::read_file(filepath));
	return _text_cache.at(filepath);
}

void asset::asset_loader::validate_path(const std::string& path)
{
	if (!std::filesystem::exists(path))
	{		
		throw std::runtime_error("Invalid path: " + path);
	}
}



