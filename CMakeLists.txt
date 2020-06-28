# Preamble
cmake_minimum_required(VERSION 3.12)
project(CS562 C CXX)

# Project wide setup
set(CS562_VERSION_MAJOR 0)
set(CS562_VERSION_MINOR 1)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake_modules")
set(EXTERNAL_INSTALL_LOCATION "${PROJECT_SOURCE_DIR}/external")
set(OpenGL_GL_PREFERENCE "GLVND")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_BINARY_DIR}/bin)
set(BUILD_SHARED_LIBS OFF)

# Hide the console window in visual studio projects
if(MSVC)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows /ENTRY:mainCRTStartup")
endif()

# Dependencies
include(add_glbinding)
include(add_nlohmann_json)
include(add_assimp)
include(add_glfw)
include(add_spdlog)
add_subdirectory(thirdparty/imgui)
add_subdirectory(thirdparty/stb)
add_subdirectory(thirdparty/freetype)
add_subdirectory(thirdparty/fmod)
add_subdirectory(thirdparty/eigen)
add_subdirectory(thirdparty/rgbe)

# Main build targets
add_subdirectory(src)
add_subdirectory(apps/project1)

# Tests
add_subdirectory(test)

# Packaging
