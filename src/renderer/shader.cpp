#include <renderer/shader.hpp>

renderer::shader::shader(gl::GLenum type, const std::string& contents)
    : _type(type)
{
    _id = glCreateShader(_type);
    from_string(contents);
    compile();
}

renderer::shader::~shader()
{
    gl::glDeleteShader(_id);
}

void renderer::shader::from_string(const std::string& shader_string)
{
    const char* c_str = shader_string.c_str();
    gl::glShaderSource(_id, 1, &c_str, nullptr);
}

void renderer::shader::compile()
{
    if (_is_compiled)
        return;

    gl::glCompileShader(_id);
    validate_shader();
}

void renderer::shader::validate_shader()
{
    using namespace gl;

    GLsizei length = 0;
    char buffer[GPU_INFOLOG_BUFFER_SIZE];

    memset(buffer, 0, GPU_INFOLOG_BUFFER_SIZE);
    glGetShaderInfoLog(_id, GPU_INFOLOG_BUFFER_SIZE, &length, buffer);

    if (length > 0)
    {
        spdlog::error(buffer);
    }
    else
    {
        _is_compiled = true;
        spdlog::info("Shader {0} compiled without issue.", _id);
    }
}