#include <cstddef>
#include <asset/assimp_model.hpp>


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