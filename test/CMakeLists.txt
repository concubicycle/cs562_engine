# ecs
add_executable(ecs_tests ecs/ecs_tests.cpp)
target_link_libraries(ecs_tests ecs)
target_include_directories(ecs_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(ecs_tests PUBLIC cxx_std_17)

# platform
add_executable(platform_tests platform/platform_tests.cpp)
target_link_libraries(platform_tests platform)
target_include_directories(platform_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(platform_tests PUBLIC cxx_std_17)

# core
add_executable(core_tests core/core_tests.cpp)
target_link_libraries(core_tests core)
target_include_directories(core_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(core_tests PUBLIC cxx_std_17)

# events
add_executable(events_tests events/events_tests.cpp)
target_link_libraries(events_tests core)
target_include_directories(events_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(events_tests PUBLIC cxx_std_17)

# renderer
add_subdirectory(renderer)