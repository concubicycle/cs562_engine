//
// Created by sava on 10/21/19.
//

#include <core/system_info.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

core::system_info::system_info()
{
    const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    _monitor_width = mode->width;
    _monitor_height = mode->height;
}