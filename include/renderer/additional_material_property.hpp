#ifndef __ADDITIONAL_MATERIAL_PROPERTY_HPP_
#define __ADDITIONAL_MATERIAL_PROPERTY_HPP_

#include <cstdint>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>

#include <renderer/program_uniforms.hpp>

#include <Eigen/Core>


namespace renderer
{	
	struct additional_material_property_f
	{
		gl::GLfloat value;
		size_t property_name_hash;

		void bind_property(const program_uniforms& uniforms);
	};

	struct additional_material_property_3f
	{
		gl::GLfloat value[3];
		size_t property_name_hash;

		void bind_property(const program_uniforms& uniforms)
		{
			gl::glUniform3f(uniforms.find_location(property_name_hash), value[0], value[1], value[2]);
		}
	};

	struct additional_material_property_4f
	{
		gl::GLfloat value[4];
		size_t property_name_hash;

		void bind_property(const program_uniforms& uniforms)
		{
			gl::glUniform4f(uniforms.find_location(property_name_hash),
				value[0],
				value[1],
				value[2],
				value[3]);
		}
	};
	 
	struct additional_material_property_mat4f
	{
		Eigen::Matrix4f value;
		size_t property_name_hash;

		void bind_property(const program_uniforms& uniforms)
		{			
			gl::glUniformMatrix4fv(uniforms.find_location(property_name_hash),
				1, gl::GL_FALSE, value.data());
		}
	};

	struct additional_material_property_i
	{
		gl::GLint value;
		size_t property_name_hash;

		void bind_property(const program_uniforms& uniforms)
		{
			gl::glUniform1i(uniforms.find_location(property_name_hash), value);
		}
	};

	struct additional_material_property_bool
	{
		gl::GLint value[3];
		size_t property_name_hash;

		void bind_property(const program_uniforms& uniforms)
		{
			gl::glUniform1i(uniforms.find_location(property_name_hash), value);
		}
	};
}

#endif