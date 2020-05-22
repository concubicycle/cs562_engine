#ifndef __OPENGL_CONTEXT__HPP_
#define __OPENGL_CONTEXT__HPP_

#include <glbinding/glbinding.h>
#include <glbinding/gl/gl.h>
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


namespace renderer
{
	class opengl_context
	{
    public:
        opengl_context();

    private:
        void initialize_opengl();
	};
}

#endif