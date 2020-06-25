#include <cstddef>
#include <asset/assimp_model.hpp>

#include <assimp/material.h>
#include <assimp/pbrmaterial.h>

asset::assimp_model::assimp_model(const aiScene* ai_scene, asset_loader& loader)
    : _ai_scene(ai_scene)
    , _loader(loader)
{
    process_node(ai_scene->mRootNode);
}

void asset::assimp_model::process_node(aiNode* node)
{
    for (size_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = _ai_scene->mMeshes[node->mMeshes[i]];
        _meshes.emplace_back(mesh);
        _mesh_materials.emplace_back(load_assimp_material(mesh));
    }
    
    for (size_t i = 0; i < node->mNumChildren; ++i)
    {
        process_node(node->mChildren[i]);
    }
}

asset::assimp_material asset::assimp_model::load_assimp_material(aiMesh* mesh)
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
        
        //aiString alphamode;
        //if (AI_SUCCESS == material->Get(AI_MATKEY_GLTF_ALPHAMODE, alphamode))
        //{
        //    int foo = 0;
        //}

        //ai_real alphacutoff;
        //if (AI_SUCCESS == material->Get(AI_MATKEY_GLTF_ALPHACUTOFF, alphacutoff))
        //{
        //    int foo = 0;
        //}

        //ai_real pbrspecularglossiness;
        //if (AI_SUCCESS == material->Get(AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS, pbrspecularglossiness))
        //{
        //    int foo = 0;
        //}

        //ai_real pbrspecularglossiness_glossiness_factor;
        //if (AI_SUCCESS == material->Get(AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS_GLOSSINESS_FACTOR, pbrspecularglossiness_glossiness_factor))
        //{
        //    int foo = 0;
        //}

        //ai_real unlit;
        //if (AI_SUCCESS == material->Get(AI_MATKEY_GLTF_UNLIT, unlit))
        //{
        //    int foo = 0;
        //}

        
        
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

void asset::assimp_model::load_material_texture(
    aiMaterial* mat, 
    aiTextureType type,
    std::unordered_map<aiTextureType, const texture_asset*>& material_textures)
{
    if (mat->GetTextureCount(type) == 0)
        return;
    
    aiString str;
    mat->GetTexture(type, 0, &str);
    std::string full_path = "assets/textures/" + std::string(str.C_Str()); 
    auto& texture_ref = _loader.get_texture(full_path);
    material_textures.insert(std::make_pair(type, &texture_ref));
}