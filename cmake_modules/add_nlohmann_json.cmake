include(FetchContent)

set (JSON_BuildTests OFF  CACHE INTERNAL "")

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG e7b3b40b5a95bc74b9a7f662830a27c49ffc01b4
)

FetchContent_MakeAvailable(nlohmann_json)
