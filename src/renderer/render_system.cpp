#include <renderer/render_system.hpp>

#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>

#include <core/glfw_context.hpp>
#include <util/debounce.hpp>

///
void gl_debug_callback(
    gl::GLenum source,
    gl::GLenum type,
    gl::GLuint id,
    gl::GLenum severity,
    gl::GLsizei length,
    const gl::GLchar* message,
    const void* user_parameter);
///


renderer::render_system::render_system()
{
    initialize_opengl();
}

void renderer::render_system::update(ecs::state& state)
{

}

void renderer::render_system::initialize_opengl()
{
    using namespace gl;

    glbinding::initialize(glfwGetProcAddress, true);

#ifndef NDEBUG
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(gl_debug_callback, nullptr);
#endif    
}




void gl_debug_callback(
    gl::GLenum source,
    gl::GLenum type,
    gl::GLuint id,
    gl::GLenum severity,
    gl::GLsizei length,
    const gl::GLchar* message,
    const void* user_parameter)
{
    using namespace gl;

    static auto print_msg = [](GLenum type, GLenum severity, const GLchar* message) {
        fprintf(stdout, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
    };

    static debounce<GLenum, GLenum, const GLchar*> print_debounce(float_second(1.f), print_msg);

    print_debounce(type, severity, message);
    if (severity > GL_DEBUG_SEVERITY_MEDIUM)
        print_msg(type, severity, message);
}