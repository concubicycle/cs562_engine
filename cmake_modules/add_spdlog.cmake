include(FetchContent)


FetchContent_Declare(spdlog
    PREFIX spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG 83b9149930f392d7797b54fe97a66ab3f2120671
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(spdlog)
