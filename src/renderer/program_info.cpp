#include <renderer/program_info.hpp>

using namespace gl;

renderer::program_info::program_info()
{
}

void renderer::program_info::initialize(int programId)
{
	_id = programId;

	glGetProgramInterfaceiv(programId, GL_PROGRAM_INPUT, GL_ACTIVE_RESOURCES, &_numAttributes);
	glGetProgramInterfaceiv(programId, GL_UNIFORM, GL_ACTIVE_RESOURCES, &_numUniforms);
	glGetProgramInterfaceiv(programId, GL_UNIFORM_BLOCK, GL_ACTIVE_RESOURCES, &_numUniformBlocks);

	std::vector<GLchar> nameData(256);
	std::vector<GLenum> properties;

	properties.push_back(GL_NAME_LENGTH);

	std::vector<GLint> values(properties.size());
	for (int attrib = 0; attrib < _numAttributes; ++attrib)
	{
		glGetProgramResourceiv(programId, GL_PROGRAM_INPUT, attrib, (GLsizei)properties.size(),
			&properties[0], (GLsizei)values.size(), NULL, &values[0]);

		nameData.resize(values[0]); //The length of the name.
		glGetProgramResourceName(programId, GL_PROGRAM_INPUT, attrib, (GLsizei)nameData.size(), NULL, &nameData[0]);
		std::string name((char*)&nameData[0]);
		GLint location = glGetAttribLocation(programId, &(nameData[0]));
		_attribLocations[name] = location;
	}

	for (int uniform = 0; uniform < _numUniforms; ++uniform)
	{
		glGetProgramResourceiv(programId, GL_UNIFORM, uniform, (GLsizei)properties.size(),
			&properties[0], (GLsizei)values.size(), NULL, &values[0]);

		nameData.resize(values[0]); //The length of the name.
		glGetProgramResourceName(programId, GL_UNIFORM, uniform, (GLsizei)nameData.size(), NULL, &nameData[0]);
		std::string name((char*)&nameData[0]);
		GLint location = glGetUniformLocation(programId, &(nameData[0]));
		_uniformLocations[name] = location;
	}

	for (int uniform_block = 0; uniform_block < _numUniformBlocks; ++uniform_block)
	{
		glGetProgramResourceiv(programId, GL_UNIFORM_BLOCK, uniform_block, (GLsizei)properties.size(),
			&properties[0], (GLsizei)values.size(), NULL, &values[0]);

		nameData.resize(values[0]); //The length of the name.
		glGetProgramResourceName(programId, GL_UNIFORM_BLOCK, uniform_block, (GLsizei)nameData.size(), NULL, &nameData[0]);
		std::string name((char*)&nameData[0]);
		GLint location = glGetUniformBlockIndex(programId, &(nameData[0]));
		_uniformBlockLocations[name] = location;
	}
}

int renderer::program_info::getAttribLocation(const std::string& name) const
{
	return _attribLocations.at(name);
}

int renderer::program_info::getUniformLocation(const std::string& name) const
{
	if (_uniformLocations.find(name) == _uniformLocations.end()) return -1;
	return _uniformLocations.at(name);
}

int renderer::program_info::getUniformBlockLocation(const std::string& name) const
{
	if (_uniformBlockLocations.find(name) == _uniformBlockLocations.end()) return -1;
	return _uniformBlockLocations.at(name);
}