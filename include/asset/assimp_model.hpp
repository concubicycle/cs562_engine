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
		- Will only read the first texture for each type
	*/
	class assimp_model
	{
	public:
		assimp_model(const aiScene* ai_scene, asset_loader& loader);

		const std::vector<assimp_mesh>& meshes() const 
		{
			return _meshes;
		}

		const std::vector<assimp_material>& mesh_materials() const 
		{
			return _mesh_materials; 
		}

		const assimp_mesh& mesh_at(size_t index)
		{
			return _meshes.at(index);
		}

		const assimp_material& material_at(size_t index)
		{
			return _mesh_materials.at(index);
		}

		size_t mesh_count() const { return _meshes.size(); }


	private:
		const aiScene* _ai_scene;
		asset_loader& _loader;

		std::vector<assimp_mesh> _meshes;
		std::vector<assimp_material> _mesh_materials; // should be index aligned with _meshes
        
		std::string _directory;

        void process_node(aiNode* node);
		asset::assimp_material load_assimp_material(aiMesh* mesh);

		void load_material_texture(
			aiMaterial* mat,
			aiTextureType type,
			std::unordered_map<aiTextureType, const texture_asset*>& material_textures);
	};
}


#endif