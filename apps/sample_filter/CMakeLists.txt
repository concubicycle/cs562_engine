add_executable(sample_filter "samplefilter.cpp")

target_link_libraries(sample_filter PRIVATE rgbe)

add_custom_target(copy_sample_filter_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

if (UNIX)
    target_link_libraries(sample_filter PRIVATE PRIVATE X11)
endif (UNIX)

if (CMAKE_DL_LIBS)
    target_link_libraries(sample_filter PRIVATE ${CMAKE_DL_LIBS})
endif ()

add_dependencies (sample_filter copy_sample_filter_assets)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(sample_filter PUBLIC OpenMP::OpenMP_CXX)
endif()