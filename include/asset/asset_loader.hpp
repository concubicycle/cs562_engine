#ifndef __ASSET_LOADER_HPP_
#define __ASSET_LOADER_HPP_

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>


namespace asset
{
	class asset_loader
	{
	public:
		nlohmann::json& get_json(const std::string& file);

	private:
		std::unordered_map<std::string, nlohmann::json> _json_cache;

		void validate_path(const std::string& path);
	};
}

#endif