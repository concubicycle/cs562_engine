set (TRANSFORMS_SOURCE_FILES 
    transform.cpp
    transform_loader.cpp
    transform_system.cpp)

add_library(transforms ${TRANSFORMS_SOURCE_FILES})
target_include_directories(transforms PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(transforms PRIVATE asset ecs)
target_compile_features(transforms PUBLIC cxx_std_17)
