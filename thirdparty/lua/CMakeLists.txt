if (WIN32)

    #Windows-specific example of imported library
    #From Professional Cmake book
    add_library(luajit SHARED IMPORTED GLOBAL)
    set_target_properties(luajit PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/lua51.dll
        IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lua51.lib
    )

    target_include_directories(luajit INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

    add_custom_target(copy_luadll
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/lua51.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lua51.dll
    )

    add_dependencies (luajit copy_luadll)

    set (LUAJIT_LIB luajit)
endif (WIN32)


if (UNIX)
    #Windows-specific example of imported library
    add_library(luajit SHARED IMPORTED GLOBAL)
    set_target_properties(luajit PROPERTIES
            IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/linux/lib/libluajit-5.1.so
            #IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/lua51.lib
            )

    target_include_directories(luajit INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

endif (UNIX)