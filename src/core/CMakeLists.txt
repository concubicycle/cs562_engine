set (CORE_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/core/framerate.cpp
        ${PROJECT_SOURCE_DIR}/src/core/frame_limiter.cpp
        ${PROJECT_SOURCE_DIR}/src/core/frame_timer.cpp
        ${PROJECT_SOURCE_DIR}/src/core/input_manager.cpp        
        ${PROJECT_SOURCE_DIR}/src/core/startup_config.cpp
        ${PROJECT_SOURCE_DIR}/src/core/system_info.cpp
        ${PROJECT_SOURCE_DIR}/src/core/cursor_state.cpp
        ${PROJECT_SOURCE_DIR}/src/core/glfw_context.cpp)

add_library(core ${CORE_HEADERS} ${CORE_SOURCE_FILES})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(core        
    nlohmann_json
    eigen
    platform
    glfw)
