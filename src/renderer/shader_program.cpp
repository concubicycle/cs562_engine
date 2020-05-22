#include <renderer/shader_program.hpp>

renderer::shader_program::shader_program(const std::string& vertex_source, const std::string& fragment_source)
	: _vertex_shader(gl::GLenum::GL_VERTEX_SHADER, vertex_source)
	, _fragment_shader(gl::GLenum::GL_FRAGMENT_SHADER, fragment_source)
{
}

void renderer::shader_program::link()
{
    using namespace gl;    

    glAttachShader(_id, _vertex_shader.id());
    glAttachShader(_id, _fragment_shader.id());
    glLinkProgram(_id);
    validate_program();
    _info.initialize(_id);
}


void renderer::shader_program::validate_program()
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
        spdlog::error("Error linking program {0}. Link error:{1} \n", _id, buffer);
    }

    glValidateProgram(_id);
    glGetProgramiv(_id, GL_VALIDATE_STATUS, &validate_status);
    if (validate_status == 0)
    {
        spdlog::error("Error validating program {0} \n.", _id);
    }
    else
    {
        glGetProgramInfoLog(_id, GPU_INFO_BUFFER_SIZE, &length, buffer);
        spdlog::info("Shader program {0} built successfully. Info log: {1}", _id, buffer);
    }
}