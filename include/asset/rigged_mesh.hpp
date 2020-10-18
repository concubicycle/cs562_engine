#ifndef __RIGGED_MESH_HPP_
#define __RIGGED_MESH_HPP_

#include <cstdint>
#include <vector>
#include <unordered_map>
#include <assimp/scene.h>
#include <asset/rigged_vertex.hpp>
#include <asset/asset_loader.hpp>
#include "bone_flattener.hpp"


namespace asset
{
  class rigged_mesh
  {
  public:
    explicit rigged_mesh(const aiScene* assimp_scene_asset, aiMesh* assimp_mesh);

    std::vector<rigged_vertex>& vertices();
    std::vector<std::uint32_t>& indices();

    size_t index_count() const;

    std::uint32_t vertex_data_bytes() const
    {
      return _vertices.size() * sizeof(rigged_vertex);
    }

    const rigged_vertex* vertex_data() const
    {
      return _vertices.data();
    }

    std::uint32_t index_data_bytes() const
    {
      return _indices.size() * sizeof(std::uint32_t);
    }

    const std::uint32_t* index_data() const
    {
      return _indices.data();
    }

  private:
    std::vector<rigged_vertex> _vertices;
    std::vector<std::uint32_t> _indices;
    std::uint32_t _num_indices{ 0 };
    std::vector<std::string> _bones_buffer;

    void build_vertices(aiMesh* mesh, asset::bone_flattener<std::string>& flattener);
    void set_bone_weights(aiMesh* mesh, asset::bone_flattener<std::string>& flattener);
  };
}
#endif