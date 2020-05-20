set (ECS_SOURCE_LIST
     ${PROJECT_SOURCE_DIR}/src/ecs/archetype_pool.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/archetype_pools.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/state.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/systems.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/world.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/chunk_component_accessor.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/entity.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/component_meta.cpp
     ${PROJECT_SOURCE_DIR}/src/ecs/query_cache.cpp)

add_library(ecs ${ECS_SOURCE_LIST})
target_include_directories(ecs PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(ecs PUBLIC cxx_std_17)
target_link_libraries(ecs PUBLIC core PRIVATE memory)
