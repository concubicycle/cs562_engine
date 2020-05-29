#include <renderer/shader_program.hpp>

#include <Eigen/Core>

renderer::shader_program::shader_program(const std::string& vertex_source, const std::string& fragment_source)
	: _vertex_shader(gl::GLenum::GL_VERTEX_SHADER, vertex_source)
	, _fragment_shader(gl::GLenum::GL_FRAGMENT_SHADER, fragment_source)
{
    _id = gl::glCreateProgram();
    link();
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

void renderer::shader_program::bind()
{
    gl::glUseProgram(_id);
}

void renderer::shader_program::unbind()
{
    gl::glUseProgram(0);
}

void renderer::shader_program::set_uniform(const std::string& name, gl::GLfloat val) const
{
    auto loc = _info.getUniformLocation(name);
    if (loc == -1) return;
    gl::glUniform1f(loc, val);
}

void renderer::shader_program::set_uniform(const std::string& name, const Eigen::Matrix4f& mat) const
{
    auto loc = _info.getUniformLocation(name);
    if (loc == -1) return;
    gl::glUniformMatrix4fv(loc, 1, gl::GL_FALSE, mat.data());
}

void renderer::shader_program::set_uniform(const std::string& name, const Eigen::Vector2f& val) const
{
    auto loc = _info.getUniformLocation(name);
    if (loc == -1) return;
    gl::glUniform2fv(loc, 1, val.data());
}

void renderer::shader_program::set_uniform(const std::string& name, gl::GLint val) const
{
    auto loc = _info.getUniformLocation(name);
    if (loc == -1) return;
    gl::glUniform1i(loc, val);
}

void renderer::shader_program::set_uniform(const std::string& name, const Eigen::Translation3f& val) const
{
    auto loc = _info.getUniformLocation(name);
    if (loc == -1) return;
    gl::glUniform3fv(loc, 1, val.vector().data());
}

void renderer::shader_program::set_uniform(const std::string& name, const Eigen::Array3f& val) const
{
    auto loc = _info.getUniformLocation(name);
    if (loc == -1) return;
    gl::glUniform3fv(loc, 1, val.data());
}

void renderer::shader_program::set_uniform(const std::string& name, const Eigen::Vector3f& val) const
{
    auto loc = _info.getUniformLocation(name);
    if (loc == -1) return;
    gl::glUniform3fv(loc, 1, val.data());
}

void renderer::shader_program::set_uniform(const std::string& name, const Eigen::Array4f& val) const
{
    auto loc = _info.getUniformLocation(name);
    if (loc == -1) return;
    gl::glUniform4fv(loc, 1, val.data());
}

void renderer::shader_program::set_uniform(gl::GLuint loc, gl::GLfloat val) const
{
    gl::glUniform1f(loc, val);
}

void renderer::shader_program::set_uniform(gl::GLuint loc, const Eigen::Matrix4f& mat) const
{
    gl::glUniformMatrix4fv(loc, 1, gl::GL_FALSE, mat.data());
}

void renderer::shader_program::set_uniform(gl::GLuint loc, const Eigen::Vector2f& val) const
{
    gl::glUniform2fv(loc, 1, val.data());
}

void renderer::shader_program::set_uniform(gl::GLuint loc, gl::GLint val) const
{
    gl::glUniform1i(loc, val);
}

void renderer::shader_program::set_uniform(gl::GLuint loc, const Eigen::Translation3f& val) const
{
    gl::glUniform3fv(loc, 1, val.vector().data());
}

void renderer::shader_program::set_uniform(gl::GLuint loc, const Eigen::Array3f& val) const
{
    gl::glUniform3fv(loc, 1, val.data());
}

void renderer::shader_program::set_uniform(gl::GLuint loc, const Eigen::Vector3f& val) const
{
    gl::glUniform3fv(loc, 1, val.data());
}

void renderer::shader_program::set_uniform(gl::GLuint loc, const Eigen::Array4f& val) const
{
    gl::glUniform4fv(loc, 1, val.data());
}