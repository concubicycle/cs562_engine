include(FetchContent)

set (OPTION_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set (OPTION_BUILD_TESTS OFF CACHE INTERNAL "")
set (OPTION_BUILD_DOCS OFF CACHE INTERNAL "")
set (OPTION_BUILD_TOOLS OFF CACHE INTERNAL "")


FetchContent_Declare(glbinding    
    GIT_REPOSITORY https://github.com/cginternals/glbinding.git
    GIT_TAG 28d32d9bbc72aedf815f18113b0bd3aa7b354108
	GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(glbinding)
