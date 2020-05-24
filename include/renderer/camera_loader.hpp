#ifndef __CAMERA_LOADER_HPP_
#define __CAMERA_LOADER_HPP_

#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>
#include <renderer/assimp_vram_loader.hpp>

namespace renderer
{
	class camera_loader : public asset::component_loader
	{
	public:
		camera_loader(asset::asset_loader& loader, assimp_vram_loader& vram_loader);

		virtual void load(asset::asset_loader_node& ecs_node) override;
		virtual component_bitset components_to_load() override;

	private:
		asset::asset_loader& _loader;
		assimp_vram_loader& _vram_loader;
	};
}

#endif
