include(FetchContent)

set (EIGEN_BUILD_TESTING OFF CACHE INTERNAL "")

FetchContent_Declare(eigen    
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.3.7
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(Eigen)
