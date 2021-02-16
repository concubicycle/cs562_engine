#ifndef __ASSIMP_MODEL_HPP_
#define __ASSIMP_MODEL_HPP_

#include <stdlib.h> 
#include <string>
#include <vector>

#include <assimp/scene.h>
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>

#include <asset/assimp_mesh.hpp>
#include <asset/asset_loader.hpp>
#include <asset/assimp_material.hpp>




namespace asset
{
  /**
    - Will only read the first texture for each type
  */
  template <typename Tmesh = assimp_mesh>
  class assimp_model
  {
  public:
    assimp_model(const aiScene* ai_scene, asset_loader& loader)
      : _ai_scene(ai_scene)
      , _loader(loader)
    {
      process_node(ai_scene->mRootNode);
    }

    const std::vector<Tmesh>& meshes() const
    {
      return _meshes;
    }

    const std::vector<assimp_material>& mesh_materials() const
    {
      return _mesh_materials;
    }

    const Tmesh& mesh_at(size_t index)
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

    std::vector<Tmesh> _meshes;
    std::vector<assimp_material> _mesh_materials; // should be index aligned with _meshes

    std::string _directory;

    void process_node(aiNode* node)
    {
      for (size_t i = 0; i < node->mNumMeshes; ++i)
      {
        aiMesh* mesh = _ai_scene->mMeshes[node->mMeshes[i]];
        _meshes.emplace_back(_ai_scene, mesh);
        _mesh_materials.emplace_back(load_assimp_material(mesh));
      }

      for (size_t i = 0; i < node->mNumChildren; ++i)
      {
        process_node(node->mChildren[i]);
      }
    }

    asset::assimp_material load_assimp_material(aiMesh* mesh)
    {
      assimp_material mat;

      if (mesh->mMaterialIndex >= 0)
      {
        aiMaterial* material = _ai_scene->mMaterials[mesh->mMaterialIndex];
        unsigned int read_buffer_size;

        ai_real pbrmetallicroughness_base_color_factor[4];
        if (AI_SUCCESS == material->Get(
          AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR,
          (ai_real*)pbrmetallicroughness_base_color_factor,
          &read_buffer_size))
        {
          memcpy(mat.base_color.data(), pbrmetallicroughness_base_color_factor, sizeof(float) * 4);
        }

        ai_real pbrmetallicroughness_metallic_factor;
        if (AI_SUCCESS == material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, pbrmetallicroughness_metallic_factor))
        {
          mat.metalness = pbrmetallicroughness_metallic_factor;
        }

        ai_real pbrmetallicroughness_roughness_factor;
        if (AI_SUCCESS == material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, pbrmetallicroughness_roughness_factor))
        {
          mat.roughness = pbrmetallicroughness_roughness_factor;
        }

        aiColor3D diffuse_color;
        if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_color))
        {
          mat.base_color = Eigen::Array4f(diffuse_color.r, diffuse_color.g, diffuse_color.b, 1);
        }

        float opacity;
        if (AI_SUCCESS == material->Get(AI_MATKEY_OPACITY, opacity))
        {
          mat.base_color = opacity;
        }

        load_material_texture(material, aiTextureType_DIFFUSE, mat.material_textures);
        load_material_texture(material, aiTextureType_SPECULAR, mat.material_textures);
        load_material_texture(material, aiTextureType_AMBIENT, mat.material_textures);
        load_material_texture(material, aiTextureType_EMISSIVE, mat.material_textures);
        load_material_texture(material, aiTextureType_HEIGHT, mat.material_textures);
        load_material_texture(material, aiTextureType_NORMALS, mat.material_textures);
        load_material_texture(material, aiTextureType_SHININESS, mat.material_textures);
        load_material_texture(material, aiTextureType_OPACITY, mat.material_textures);
        load_material_texture(material, aiTextureType_DISPLACEMENT, mat.material_textures);
        load_material_texture(material, aiTextureType_LIGHTMAP, mat.material_textures);
        load_material_texture(material, aiTextureType_REFLECTION, mat.material_textures);
        load_material_texture(material, aiTextureType_BASE_COLOR, mat.material_textures);
        load_material_texture(material, aiTextureType_NORMAL_CAMERA, mat.material_textures);
        load_material_texture(material, aiTextureType_EMISSION_COLOR, mat.material_textures);
        load_material_texture(material, aiTextureType_METALNESS, mat.material_textures);
        load_material_texture(material, aiTextureType_DIFFUSE_ROUGHNESS, mat.material_textures);
        load_material_texture(material, aiTextureType_AMBIENT_OCCLUSION, mat.material_textures);
      }

      return mat;
    }


    void load_material_texture(
      aiMaterial* mat,
      aiTextureType type,
      std::unordered_map<aiTextureType, const texture_asset*>& material_textures)
    {
      if (mat->GetTextureCount(type) == 0)
        return;

      aiString str;
      mat->GetTexture(type, 0, &str);

      if (str.data[0] == '*')
      {
        // embedded texture
        throw std::runtime_error("Embedded model textuers unsupported for now.");
      }
      else
      {
        // external texture
        std::string full_path = "assets/textures/" + std::string(str.C_Str());
        auto& texture_ref = _loader.get_texture(full_path);
        material_textures.insert(std::make_pair(type, &texture_ref));
      }
    }
  };
}


#endif