#ifndef __MATERIAL_HPP_
#define __MATERIAL_HPP_

#include <optional>
#include <cstdint>
#include <renderer/opengl_texture.hpp>
#include <renderer/additional_material_property.hpp>


namespace renderer
{
	struct opengl_material
	{
		static constexpr std::uint8_t MaxAdditionalFloats = 1;
		static constexpr std::uint8_t MaxAdditionalVec3s = 1;
		static constexpr std::uint8_t MaxAdditionalMat4s = 1;

		opengl_texture diffuse_texture{gl::GL_TEXTURE0};
		opengl_texture metalness_texture{ gl::GL_TEXTURE0 + 1 };
		opengl_texture normal_texture{ gl::GL_TEXTURE0 + 2 };
		opengl_texture roughness_texture{ gl::GL_TEXTURE0 + 3 };
		opengl_texture ambient_occlusion_texture{ gl::GL_TEXTURE0 + 4 };
				
		float roughness{ 1 };
		Eigen::Array3f fresnel_color{ 0.562, 0.565, 0.578 }; // iron



		bool has_additional {false};

		additional_material_property_f additional_floats[MaxAdditionalFloats];
		std::uint8_t addition_float_count{ 0 };

		additional_material_property_3f additional_vec3s[MaxAdditionalVec3s];
		std::uint8_t additional_vec3_count{ 0 };

		additional_material_property_mat4f additional_mat4s[MaxAdditionalMat4s];
		std::uint8_t additional_mat4_count{ 0 };

		void add_additional_float(additional_material_property_f value)
		{
			if (addition_float_count == MaxAdditionalFloats)
			{
				throw std::runtime_error("additional property count exceeded");
			}

			additional_floats[addition_float_count++] = value;
			has_additional = true;
		}

		void add_additional_vec3(additional_material_property_3f value)
		{
			if (additional_vec3_count == MaxAdditionalVec3s)
			{
				throw std::runtime_error("additional property count exceeded");
			}

			additional_vec3s[additional_vec3_count++] = value;
			has_additional = true;
		}

		void add_additional_mat4(additional_material_property_mat4f value)
		{
			if (additional_mat4_count == MaxAdditionalMat4s)
			{
				throw std::runtime_error("additional property count exceeded");
			}

			additional_mat4s[additional_mat4_count++] = value;
			has_additional = true;
		}
	};
}


#endif