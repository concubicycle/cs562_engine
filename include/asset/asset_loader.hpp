#ifndef __ASSET_LOADER_HPP_
#define __ASSET_LOADER_HPP_

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include <asset/assimp_scene_asset.hpp>

namespace asset
{
	class asset_loader
	{
	public:
		asset_loader();
		~asset_loader();

		nlohmann::json& get_json(const std::string& file);
		const assimp_scene_asset& get_assimp_scene(const std::string& file);

	private:
		std::unordered_map<std::string, nlohmann::json> _json_cache;
		std::unordered_map<std::string, assimp_scene_asset> _assimp_cache;
		
		void validate_path(const std::string& path);
	};
}

#endif