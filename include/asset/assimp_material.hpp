#ifndef __ASSIMP_MATERIAL_HPP_
#define __ASSIMP_MATERIAL_HPP_

#include <vector>
#include <asset/texture_asset.hpp>
#include <unordered_map>
#include <assimp/scene.h>

namespace asset
{
	struct assimp_material
	{
		std::unordered_map<aiTextureType, const texture_asset*> material_textures;
	};
}



#endif