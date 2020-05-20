#include <asset/assimp_mesh.hpp>

asset::assimp_mesh::assimp_mesh(aiMesh* mesh)
{
    build_vertices(mesh);
}

void asset::assimp_mesh::build_vertices(aiMesh* mesh)
{
    // Reorganize data into array of struct instead of separate arrays
    assimp_vertex swap_vertex;
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        swap_vertex.position[0] = mesh->mVertices[i].x;
        swap_vertex.position[1] = mesh->mVertices[i].y;
        swap_vertex.position[2] = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            swap_vertex.normal[0] = mesh->mNormals[i].x;
            swap_vertex.normal[1] = mesh->mNormals[i].y;
            swap_vertex.normal[2] = mesh->mNormals[i].z;
        }
        if (mesh->HasTextureCoords(0))
        {
            swap_vertex.texture_coords[0] = mesh->mTextureCoords[0][i].x;
            swap_vertex.texture_coords[1] = mesh->mTextureCoords[0][i].y;
        }
        if (mesh->HasVertexColors(0))
        {
            swap_vertex.color[0] = mesh->mColors[0][i].r;
            swap_vertex.color[1] = mesh->mColors[0][i].g;
            swap_vertex.color[2] = mesh->mColors[0][i].b;
            swap_vertex.color[3] = mesh->mColors[0][i].a;
        }
        if (mesh->HasTangentsAndBitangents())
        {
            swap_vertex.tangent[0] = mesh->mTangents[i].x;
            swap_vertex.tangent[1] = mesh->mTangents[i].y;
            swap_vertex.tangent[2] = mesh->mTangents[i].z;
            swap_vertex.bitangent[0] = mesh->mBitangents[i].x;
            swap_vertex.bitangent[1] = mesh->mBitangents[i].y;
            swap_vertex.bitangent[2] = mesh->mBitangents[i].z;
        }
        _vertices.push_back(swap_vertex);
    }
    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        _indices.emplace_back(mesh->mFaces[i].mIndices[0]);
        _indices.emplace_back(mesh->mFaces[i].mIndices[1]);
        _indices.emplace_back(mesh->mFaces[i].mIndices[2]);
    }

    _num_indices = _indices.size();
}