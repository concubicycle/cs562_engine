add_executable(project1 main.cpp "run_app.cpp")
target_link_libraries(project1 PRIVATE
	core
	ecs
	asset
	transforms	
	renderer
	engineui)

target_compile_features(project1 PUBLIC cxx_std_17)

add_custom_target(copy_project1_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

if (UNIX)
    target_link_libraries(project1 PRIVATE PRIVATE X11)
endif (UNIX)

if (CMAKE_DL_LIBS)
    target_link_libraries(project1 PRIVATE ${CMAKE_DL_LIBS})
endif ()

add_dependencies (project1 copy_project1_assets)

