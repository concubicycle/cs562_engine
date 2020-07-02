#ifndef __ASSET_LOADER_HPP_
#define __ASSET_LOADER_HPP_

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include <asset/assimp_scene_asset.hpp>
#include <asset/texture_asset.hpp>


namespace asset
{
	class asset_loader
	{
	public:
		asset_loader();
		~asset_loader();

		nlohmann::json& get_json(const std::string& file);
		const assimp_scene_asset& get_assimp_scene(const std::string& file);
		const texture_asset& get_texture(const std::string& file);
		const texture_assetf& get_texturef(const std::string& file);
		const texture_assetf& get_texturef_untonemapped(const std::string& file);
		const std::string& get_text(const std::string& file);

	private:
		std::unordered_map<std::string, nlohmann::json> _json_cache;
		std::unordered_map<std::string, assimp_scene_asset> _assimp_cache;
		std::unordered_map<std::string, texture_asset> _texture_cache;
		std::unordered_map<std::string, texture_assetf> _hdr_texture_cache;
		std::unordered_map<std::string, std::string> _text_cache;

		
		void validate_path(const std::string& path);
	};
}

#endif