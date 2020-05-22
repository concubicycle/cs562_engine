#ifndef __SHADER_PROGRAM_HPP_
#define __SHADER_PROGRAM_HPP_

#include <string>
#include <glbinding/gl/gl.h>

#include <renderer/shader.hpp>
#include <renderer/program_info.hpp>

namespace renderer
{
	class shader_program
	{		
        static constexpr unsigned int GPU_INFO_BUFFER_SIZE = 512;

	public:
		shader_program(const std::string& vertex_source, const std::string& fragment_source);

	private:
		shader _vertex_shader;
		shader _fragment_shader;

        gl::GLuint _id;
		program_info _info;

		void link();
		void validate_program();
	};
}



#endif