#ifndef __ASSIMP_MESH_HPP_
#define __ASSIMP_MESH_HPP_

#include <cstdint>
#include <vector>
#include <assimp/scene.h>
#include <asset/assimp_material.hpp>
#include <asset/assimp_vertex.hpp>

namespace asset
{
	class assimp_mesh
	{
	public:
		assimp_mesh(aiMesh* mesh);

		std::uint32_t vertex_data_bytes() const
		{
			return _vertices.size() * sizeof(assimp_vertex);
		}

		const assimp_vertex* vertex_data() const
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
		std::vector<assimp_vertex> _vertices;
		std::vector<std::uint32_t> _indices;
		std::uint32_t _num_indices{ 0 };

		void build_vertices(aiMesh* mesh);
	};
}


#endif