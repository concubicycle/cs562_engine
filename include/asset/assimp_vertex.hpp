#ifndef __ASSIMP_VERTEX_HPP_
#define __ASSIMP_VERTEX_HPP_

#include <Eigen/Core>

namespace asset
{
	struct assimp_vertex
	{
		Eigen::Vector3f position;
		Eigen::Vector3f normal{ 0.f, 1.f, 0.f };
		Eigen::Vector3f tangent{ 1.0, 0.f, 0.f };
		Eigen::Vector3f bitangent{ 0.0, 0.f, 1.f };
		Eigen::Array4f color{ 1.f, 0.f, 1.f, 1.f };
		Eigen::Array2f texture_coords;
	};
}


#endif