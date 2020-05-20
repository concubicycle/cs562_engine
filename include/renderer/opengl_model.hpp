#ifndef __OPENGL_MODEL_HPP_
#define __OPENGL_MODEL_HPP_

#include <cstdint>
#include <renderer/opengl_mesh.hpp>


namespace renderer
{
	struct opengl_model
	{
		static constexpr std::uint16_t MaxMeshCount = 2048;
		opengl_mesh meshes[MaxMeshCount];
		std::uint16_t mesh_count{ 0 };
	};
}


#endif