#ifndef __ASSIMP_MODEL_HPP_
#define __ASSIMP_MODEL_HPP_

#include <string>
#include <vector>
#include <assimp/scene.h>
#include <asset/assimp_mesh.hpp>
#include <asset/asset_loader.hpp>
#include <asset/assimp_material.hpp>

namespace asset
{
	/**
	Will only read the first texture for each type
	*/
	class assimp_model
	{
	public:
		assimp_model(const aiScene* ai_scene, asset_loader& loader);

	private:
		const aiScene* _ai_scene;
		asset_loader& _loader;

		std::vector<assimp_mesh> _meshes;
		std::vector<assimp_material> _meshe_materials; // should be index aligned with _meshes
        
        void process_node(aiNode* node);
		asset::assimp_material load_assimp_material(aiMesh* mesh);

		void asset::assimp_model::load_mat_texture(
			aiMaterial* mat,
			aiTextureType type,
			std::unordered_map<aiTextureType, const texture_asset*>& material_textures);
	};
}


#endif