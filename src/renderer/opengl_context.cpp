#include <renderer/opengl_context.hpp>
#include <core/glfw_context.hpp>
#include <iostream>
#include <unordered_map>
#include <spdlog/spdlog.h>

#include <glbinding-aux/Meta.h>


renderer::opengl_context::opengl_context()
{
	initialize_opengl();
}

void renderer::opengl_context::initialize_opengl()
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


	static std::unordered_map<GLenum, std::string> types
	{

	};


	static auto print_msg = [](GLenum type, GLenum severity, const GLchar* message) {

		switch (severity)
		{
		case GLenum::GL_DEBUG_SEVERITY_LOW:
		case GLenum::GL_DEBUG_SEVERITY_MEDIUM:
			spdlog::warn(
				"GL CALLBACK: type = {}, severity = {}, message = {}\n\n",
				glbinding::aux::Meta::getString(type),
				glbinding::aux::Meta::getString(severity),
				message);
			break;
		case GLenum::GL_DEBUG_SEVERITY_HIGH:
			spdlog::warn(
				"GL CALLBACK: type = {}, severity = {}, message = {}\n\n",
				glbinding::aux::Meta::getString(type),
				glbinding::aux::Meta::getString(severity),
				message);
			break;
		default:
			spdlog::info(
				"GL CALLBACK: type = {}, severity = {}, message = {}\n\n",
				glbinding::aux::Meta::getString(type),
				glbinding::aux::Meta::getString(severity),
				message);
		}
	};

	// avoid spammy non-error messages
	static debounce<GLenum, GLenum, const GLchar*> print_debounce(float_second(1.f), print_msg);
	print_debounce(type, severity, message);

	if (severity > GL_DEBUG_SEVERITY_NOTIFICATION)
		print_msg(type, severity, message);
}