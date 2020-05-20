include(FetchContent)

set (GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set (GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set (GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")

# When other deps (Eigen) declare an 'uninstall' target,
# it will conflict with this one. Luckily, GLFW lets you
# turn the install/uninstall target off. 
set (GLFW_INSTALL OFF CACHE INTERNAL "")

FetchContent_Declare(glfw    
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG ecda86fa4f89ecdd364e5a1a22645030fe0ced6e)


FetchContent_MakeAvailable(glfw)

