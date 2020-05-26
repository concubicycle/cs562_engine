set(ENGINEUI_SOURCE_LIST
    fps_display.cpp
    imgui_overlay.cpp
    )

add_library(engineui ${ENGINEUI_SOURCE_LIST})
target_include_directories(engineui PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(engineui imgui core glfw)
target_compile_features(engineui PUBLIC cxx_std_17)