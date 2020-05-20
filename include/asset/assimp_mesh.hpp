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

	private:
		std::vector<assimp_vertex> _vertices;
		std::vector<std::uint32_t> _indices;
		std::uint32_t _num_indices{ 0 };

		void build_vertices(aiMesh* mesh);
	};
}


#endif