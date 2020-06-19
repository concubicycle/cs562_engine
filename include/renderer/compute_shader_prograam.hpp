#ifndef __COMPUTE_SHADER_PROGRAM_HPP_
#define __COMPUTE_SHADER_PROGRAM_HPP_

#include <renderer/shader.hpp>
#include <renderer/shader_program.hpp>

namespace renderer
{
	class compute_shader_program : public shader_program_base
	{
		static constexpr unsigned int GPU_INFO_BUFFER_SIZE = 512;

	public:
		compute_shader_program(const std::string& source);


	private:
		shader _shader;

		void link();

		void validate_program();

	};
}


#endif