if (WIN32)

    set (FREETYPE_SO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/windows/freetype.dll PARENT_SCOPE)

    #Windows-specific example of imported library
    #From Professional Cmake book
    add_library(freetype SHARED IMPORTED GLOBAL)
    set_target_properties(freetype PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/windows/freetype.dll
        IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/windows/freetype.lib
    )

    target_include_directories(freetype INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

    add_custom_target(copy_freetypedll
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/windows/freetype.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/freetype.dll
    )

    add_dependencies (freetype copy_freetypedll)

    set (FREETYPE_LIB freetype)
endif (WIN32)


if (UNIX)    

    set (FREETYPE_SO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linux/libfreetype.so PARENT_SCOPE)

    add_library(freetype SHARED IMPORTED GLOBAL)
    set_target_properties(freetype PROPERTIES
            IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/linux/libfreetype.so)

    target_include_directories(freetype INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    #target_include_directories(freetype PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/linux/include/freetype/config)

endif (UNIX)