#include <vector>
#include <asset/rigged_mesh.hpp>
#include <asset/bone_flattener.hpp>




asset::rigged_mesh::rigged_mesh(const aiScene* assimp_scene, aiMesh* assimp_mesh)
{
    size_t num_nodes = 0;
    bone_flattener<std::string>::count_nodes(assimp_scene->mRootNode, num_nodes);
    _bones_buffer.resize(num_nodes);
    asset::bone_flattener<std::string> flattener(
            assimp_scene,
            _bones_buffer.data(),
            num_nodes,
            [&] (
                const aiNode* ai_node,
                std::string* node,
                std::string* parent,
                bone_flattener<std::string>& flattener) {

            });

    build_vertices(assimp_mesh, flattener);
    set_bone_weights(assimp_mesh, flattener);
}

void asset::rigged_mesh::build_vertices(aiMesh *assimp_mesh, asset::bone_flattener<std::string>& flattener)
{
    // Reorganize data into array of struct instead of separate arrays
    rigged_vertex swap_vertex;
    for (size_t i = 0; i < assimp_mesh->mNumVertices; i++)
    {
        swap_vertex.position[0] = assimp_mesh->mVertices[i].x;
        swap_vertex.position[1] = assimp_mesh->mVertices[i].y;
        swap_vertex.position[2] = assimp_mesh->mVertices[i].z;

        if (assimp_mesh->HasNormals())
        {
            swap_vertex.normal[0] = assimp_mesh->mNormals[i].x;
            swap_vertex.normal[1] = assimp_mesh->mNormals[i].y;
            swap_vertex.normal[2] = assimp_mesh->mNormals[i].z;
        }
        if (assimp_mesh->HasTextureCoords(0))
        {
            swap_vertex.tex_coord[0] = assimp_mesh->mTextureCoords[0][i].x;
            swap_vertex.tex_coord[1] = assimp_mesh->mTextureCoords[0][i].y;
        }
        if (assimp_mesh->HasTangentsAndBitangents())
        {
            swap_vertex.tangent[0] = assimp_mesh->mTangents[i].x;
            swap_vertex.tangent[1] = assimp_mesh->mTangents[i].y;
            swap_vertex.tangent[2] = assimp_mesh->mTangents[i].z;
            swap_vertex.bitangent[0] = assimp_mesh->mBitangents[i].x;
            swap_vertex.bitangent[1] = assimp_mesh->mBitangents[i].y;
            swap_vertex.bitangent[2] = assimp_mesh->mBitangents[i].z;
        }
        _vertices.push_back(swap_vertex);
    }
    for (size_t i = 0; i < assimp_mesh->mNumFaces; i++)
    {
        _indices.emplace_back(assimp_mesh->mFaces[i].mIndices[0]);
        _indices.emplace_back(assimp_mesh->mFaces[i].mIndices[1]);
        _indices.emplace_back(assimp_mesh->mFaces[i].mIndices[2]);
    }
    _num_indices = _indices.size();
}

void asset::rigged_mesh::set_bone_weights(aiMesh *mesh, asset::bone_flattener<std::string>& flattener)
{
    for (std::uint32_t bone_index = 0; bone_index < mesh->mNumBones; ++bone_index)
    {
        auto& bone = mesh->mBones[bone_index];
        std::string bone_name = bone->mName.data;
        auto bone_id = flattener.find_node_index(bone_name);

        for (std::uint32_t weight_index = 0; weight_index < bone->mNumWeights; ++weight_index)
        {
            auto& weight = bone->mWeights[weight_index];
            auto& vertex = _vertices[weight.mVertexId];
            vertex.add_weight(bone_id, weight.mWeight);
        }
    }
}

std::vector<asset::rigged_vertex> &asset::rigged_mesh::vertices()
{
    return _vertices;
}

std::vector<std::uint32_t > &asset::rigged_mesh::indices()
{
    return _indices;
}
