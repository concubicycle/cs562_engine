set (RENDERER_SOURCE_FILES
    render_system.cpp
    model_instance.cpp
    uniform_location_lookup.cpp
    program_uniforms.cpp
    additional_material_property.cpp
    model_loader.cpp 
    assimp_vram_loader.cpp 
    shader.cpp
    shader_program.cpp
    program_info.cpp
    camera.cpp
    punctual_light.cpp
    camera_update_system.cpp
    punctual_light_loader.cpp
    camera_loader.cpp 
    opengl_context.cpp
    ambient_light.cpp
    ambient_light_loader.cpp
     )

add_library(renderer ${RENDERER_SOURCE_FILES})
target_include_directories(renderer PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(renderer 
    PRIVATE ecs transforms core asset glfw eigen
    PUBLIC glbinding glbinding-aux) # not sure why glbinding needs to be public. headers can't see it otherwise. 

target_compile_features(renderer PUBLIC cxx_std_17)

add_custom_target(copy_render_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/renderer/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PROJECT_SOURCE_DIR}/src/renderer/fonts ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets/fonts)

add_dependencies (renderer copy_render_assets)
