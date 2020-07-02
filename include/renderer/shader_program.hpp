#ifndef __SHADER_PROGRAM_HPP_
#define __SHADER_PROGRAM_HPP_

#include <string>
#include <glbinding/gl/gl.h>

#include <renderer/shader.hpp>
#include <renderer/program_info.hpp>

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace renderer
{
	class shader_program_base
	{
	public:
		void set_uniform(const std::string& name, const gl::GLfloat val) const;
		void set_uniform(const std::string& name, const Eigen::Matrix4f& mat) const;
		void set_uniform(const std::string& name, const Eigen::Vector3f& val) const;
		void set_uniform(const std::string& name, const Eigen::Vector2f& val) const;
		void set_uniform(const std::string& name, const gl::GLint val) const;
		void set_uniform(const std::string& name, const Eigen::Translation3f& val) const;
		void set_uniform(const std::string& name, const Eigen::Array2f& val) const;
		void set_uniform(const std::string& name, const Eigen::Array3f& val) const;
		void set_uniform(const std::string& name, const Eigen::Array4f& val) const;


		void set_uniform(gl::GLuint loc, const gl::GLfloat val) const;
		void set_uniform(gl::GLuint loc, const Eigen::Matrix4f& mat) const;
		void set_uniform(gl::GLuint loc, const Eigen::Vector3f& val) const;
		void set_uniform(gl::GLuint loc, const Eigen::Vector2f& val) const;
		void set_uniform(gl::GLuint loc, const gl::GLint val) const;
		void set_uniform(gl::GLuint loc, const Eigen::Translation3f& val) const;
		void set_uniform(gl::GLuint loc, const Eigen::Array3f& val) const;
		void set_uniform(gl::GLuint loc, const Eigen::Array4f& val) const;

		void bind_uniform_block(const std::string& name, gl::GLuint bindpoint) const;

		void bind();
		void unbind();

		gl::GLuint uniform_location(const std::string& name) const;

	protected:
		gl::GLuint _id;
		program_info _info;

	};

	class shader_program : public shader_program_base
	{		
        static constexpr unsigned int GPU_INFO_BUFFER_SIZE = 512;

	public:
		shader_program(const std::string& vertex_source, const std::string& fragment_source);

	private:
		shader _vertex_shader;
		shader _fragment_shader;

		void link();
		void validate_program();
	};
}



#endif