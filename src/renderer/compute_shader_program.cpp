#include <renderer/compute_shader_prograam.hpp>

renderer::compute_shader_program::compute_shader_program(const std::string& source)
	: _shader(gl::GLenum::GL_COMPUTE_SHADER, source)
{
	_id = gl::glCreateProgram();
    link();
}

void renderer::compute_shader_program::link()
{
    using namespace gl;

    glAttachShader(_id, _shader.id());    
    glLinkProgram(_id);
    validate_program();
    _info.initialize(_id);
}


void renderer::compute_shader_program::validate_program()
{
    using namespace gl;

    char buffer[GPU_INFO_BUFFER_SIZE];
    GLsizei length = 0;
    GLint link_status;
    GLint validate_status;

    memset(buffer, 0, GPU_INFO_BUFFER_SIZE);

    glGetProgramiv(_id, GL_LINK_STATUS, &link_status);
    if (!link_status)
    {
        glGetProgramInfoLog(_id, GPU_INFO_BUFFER_SIZE, &length, buffer);
        spdlog::error("Error linking compute shader program {0}. Link error:{1} \n", _id, buffer);
    }

    glValidateProgram(_id);
    glGetProgramiv(_id, GL_VALIDATE_STATUS, &validate_status);
    if (validate_status == 0)
    {
        spdlog::error("Error validating compute shader program {0} \n.", _id);
    }
    else
    {
        glGetProgramInfoLog(_id, GPU_INFO_BUFFER_SIZE, &length, buffer);
        spdlog::info("Compute shader program {0} built successfully. Info log: {1}", _id, buffer);
    }
}