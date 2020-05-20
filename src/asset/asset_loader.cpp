#include <asset/asset_loader.hpp>

#include <filesystem>
#include <fstream>

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

void asset::asset_loader::validate_path(const std::string& path)
{
	if (!std::filesystem::exists(path))
	{		
		throw std::runtime_error("Invalid path: " + path);
	}
}
