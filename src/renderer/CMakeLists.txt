set (RENDERER_SOURCE_FILES
    render_system.cpp
    model_instance.cpp
    uniform_location_lookup.cpp
    program_uniforms.cpp
    additional_material_property.cpp
 )

add_library(renderer ${RENDERER_SOURCE_FILES})
target_include_directories(renderer PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(renderer 
    PRIVATE ecs transforms core 
    PUBLIC glbinding) # not sure why glbinding needs to be public. headers can't see it otherwise. 

target_compile_features(renderer PUBLIC cxx_std_17)