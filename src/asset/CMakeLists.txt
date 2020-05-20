set (ASSET_SOURCE_LIST
        
        ${PROJECT_SOURCE_DIR}/src/asset/scene.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_entity.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/resource_id.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_hydrater.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/model_loader.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/rigged_mesh.cpp
        ${PROJECT_SOURCE_DIR}/src/asset/scene_tracker.cpp 
        asset_loader.cpp)

add_library(asset STATIC ${ASSET_SOURCE_LIST})
target_include_directories(asset PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(asset PUBLIC cxx_std_17)
target_link_libraries(asset PUBLIC core ecs assimp fmod stb_image)