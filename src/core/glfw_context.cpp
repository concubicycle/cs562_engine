#include <core/glfw_context.hpp>


core::glfw_context::glfw_context(startup_config& conf) : _conf(conf)
{
	if (!glfwInit())
	{
		auto msg = "Error: GLFW failed to initialize.";
		std::cerr << msg << std::endl;
		throw std::runtime_error(msg);
	}

	uint32_t window_width = conf.width();
	uint32_t window_height = conf.height();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

	_monitor = glfwGetPrimaryMonitor();
	_mode = glfwGetVideoMode(_monitor);
	glfwWindowHint(GLFW_RED_BITS, _mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, _mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, _mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, _mode->refreshRate);

	glfwWindowHint(GLFW_ALPHA_BITS, 32);
	glfwWindowHint(GLFW_DEPTH_BITS, 32);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);

#ifndef NDEBUG	
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    if (conf.fullscreen())
    {
        _window = glfwCreateWindow(_mode->width, _mode->height, conf.window_title().c_str(), _monitor, NULL);
		_width = _mode->width;
		_height = _mode->height;
    } else
    {
        _window = glfwCreateWindow(conf.width(), conf.height(), conf.window_title().c_str(), nullptr, nullptr);
		_width = conf.width();
		_height = conf.height();
    }

	if (!_window)
	{
		auto msg = "Error: GLFW failed to create window.";
		std::cerr << msg << std::endl;
		throw std::runtime_error(msg);
	}

	glfwMakeContextCurrent(_window);
	
	if (!conf.free_mouse())
		glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

core::glfw_context::~glfw_context()
{
	glfwTerminate();
}

GLFWwindow* core::glfw_context::window()
{
	return _window;
}

const GLFWwindow* core::glfw_context::window() const
{
	return _window;
}

void core::glfw_context::swap_buffers()
{
	glfwSwapBuffers(_window);
}

std::uint32_t core::glfw_context::width() const
{
	return _width;
}

std::uint32_t core::glfw_context::height() const
{
	return _height;
}

void core::glfw_context::set_should_close(bool value)
{
	glfwSetWindowShouldClose(_window, value);
}


void core::glfw_context::set_minimize_callback(GLFWwindowiconifyfun callback)
{
	glfwSetWindowIconifyCallback(_window, callback);
}

bool core::glfw_context::is_minimized()
{
	return glfwGetWindowAttrib(_window, GLFW_ICONIFIED);
}

void core::glfw_context::set_fullscreen(bool val)
{
	if (val)
	{
		glfwSetWindowMonitor(_window, _monitor, 0, 0, _mode->width, _mode->height, _mode->refreshRate);
		_width = _mode->width;
		_height = _mode->height;
	}
	else
	{
		glfwSetWindowMonitor(_window, NULL, 64, 64, _conf.width(), _conf.height(), 0);
		_width = _conf.width();
		_height = _conf.height();
	}	
}

bool core::glfw_context::is_fullscreen() {
	return glfwGetWindowMonitor(_window) != NULL;
}

