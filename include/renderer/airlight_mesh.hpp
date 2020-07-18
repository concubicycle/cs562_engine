#ifndef __AIRLIGHT_MESH_HPP_
#define __AIRLIGHT_MESH_HPP_

#include <vector>
#include <cstdint>
#include <Eigen/Core>
#include <glbinding/gl/gl.h>



namespace renderer
{
	class airlight_mesh
	{
	public:
		airlight_mesh(std::uint32_t shadowmap_width, std::uint32_t shadowmap_height);

		void draw();

	private:
		std::vector<Eigen::Vector3f> _vertices;
		std::vector<std::uint32_t> _indices;

		unsigned int vao = 0;
		unsigned int vbo;
		unsigned int ebo;

		void build_mesh(
			std::uint32_t shadowmap_width,
			std::uint32_t shadowmap_height);

		size_t vertices_size_bytes() const;
		size_t indices_size_bytes() const;

	};

}

#endif