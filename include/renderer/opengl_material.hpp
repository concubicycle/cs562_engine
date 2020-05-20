#ifndef __MATERIAL_HPP_
#define __MATERIAL_HPP_

#include <cstdint>
#include <renderer/material_texture.hpp>
#include <renderer/additional_material_property.hpp>


namespace renderer
{
	struct opengl_material
	{
		static constexpr std::uint8_t MaxAdditionalFloats = 8;
		static constexpr std::uint8_t MaxAdditionalVec3s = 4;
		static constexpr std::uint8_t MaxAdditionalMat4s = 4;
		static constexpr std::uint8_t MaxAdditionalInts = 8;
		static constexpr std::uint8_t MaxAdditionalBools = 8;

		material_texture diffuse_texture{gl::GL_TEXTURE0};
		material_texture metalness_texture{ gl::GL_TEXTURE0 + 1 };
		material_texture normal_texture{ gl::GL_TEXTURE0 + 2 };
		material_texture roughness_texture{ gl::GL_TEXTURE0 + 3 };
		material_texture ambient_occlusion_texture{ gl::GL_TEXTURE0 + 4 };

		additional_material_property_f additional_floats[MaxAdditionalFloats];
		std::uint8_t addition_float_count{ 0 };

		additional_material_property_3f additional_vec3[MaxAdditionalVec3s];
		std::uint8_t additional_vec3_count{ 0 };

		additional_material_property_mat4f additional_mat4[MaxAdditionalMat4s];
		std::uint8_t additional_mat4_count{ 0 };

		additional_material_property_i additional_ints[MaxAdditionalInts];
		std::uint8_t additional_int_count{ 0 };

		additional_material_property_bool additional_bools[MaxAdditionalBools];
		std::uint8_t additional_bool_count{ 0 };
	};
}


#endif