#ifndef __SHADER_PROGRAM_INFO__
#define __SHADER_PROGRAM_INFO__

#include <glbinding/gl/gl.h>
#include <vector>
#include <map>
#include <string>

namespace renderer
{

	class program_info
	{
	private:
		std::uint32_t _id;

		int _numUniforms = 0;
		int _numAttributes = 0;
		int _numUniformBlocks = 0;

		std::map<std::string, int> _attribLocations;
		std::map<std::string, int> _uniformLocations;
		std::map<std::string, int> _uniformBlockLocations;

	public:
		program_info();
		void initialize(int programId);

		int getAttribLocation(const std::string& name) const;
		int getUniformLocation(const std::string& name) const;
		int getUniformBlockLocation(const std::string& name) const;

		std::uint32_t id() const { return _id; }
	};
} 

#endif