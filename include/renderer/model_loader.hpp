#ifndef __MODEL_LOADER_HPP_
#define __MODEL_LOADER_HPP_

#include <asset/component_loader.hpp>
#include <asset/asset_loader_node.hpp>
#include <util/string_table.hpp>

#include <renderer/assimp_vram_loader.hpp>

namespace renderer
{
	class model_loader : public asset::component_loader
	{
	public:
		model_loader(
			util::string_table& strings, 
			asset::asset_loader& asset_loader,
			assimp_vram_loader& vram_loader);

		virtual void load(asset::asset_loader_node& ecs_node) override;
		virtual component_bitset components_to_load() override;

	private:
		util::string_table _strings;
		asset::asset_loader& _asset_loader;
		assimp_vram_loader& _vram_loader;

		void assign_material(opengl_material& material, json definition);
	};
}


#endif