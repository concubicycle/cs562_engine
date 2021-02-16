set (PHYSICS_SOURCE_FILES
        point_cloud.cpp "point_mass.cpp" "rk4.cpp" "rigid_body.cpp" "physics_update.cpp" "rigid_body_loader.cpp" "spring.cpp")

add_library(physics ${PHYSICS_SOURCE_FILES} "model.cpp")
target_include_directories(physics PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(physics PUBLIC    
    asset
    math
    ecs
    nlohmann_json
    eigen
    spdlog)
