#ifndef __PUNCTUAL_LIGHT_LOADER_HPP_
#define __PUNCTUAL_LIGHT_LOADER_HPP_

#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>
#include <util/string_table.hpp>

#include <renderer/assimp_vram_loader.hpp>

namespace renderer
{
	class punctual_light_loader : public asset::component_loader
	{
	public:
		virtual void load(asset::asset_loader_node& ecs_node) override;
		virtual component_bitset components_to_load() override;
	};
}

#endif
