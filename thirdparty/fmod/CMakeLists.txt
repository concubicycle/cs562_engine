if (WIN32)

    set (FMOD_SO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/windows/Win64/fmod.dll PARENT_SCOPE)

    #Windows-specific example of imported library
    #From Professional Cmake book
    add_library(fmod SHARED IMPORTED GLOBAL)
    set_target_properties(fmod PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/windows/Win64/fmod.dll
        IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/windows/Win64/fmod_vc.lib
    )

    add_library(fmodL SHARED IMPORTED GLOBAL)
    set_target_properties(fmodL PROPERTIES
            IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/windows/Win64/fmodL.dll
            IMPORTED_IMPLIB ${CMAKE_CURRENT_SOURCE_DIR}/windows/Win64/fmodL_vc.lib
    )

    target_include_directories(fmod INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/windows/FMOD)
    target_include_directories(fmodL INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/windows/FMOD)

    add_custom_target(copy_fmod
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/windows/Win64/fmod.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fmod.dll)
    add_custom_target(copy_fmodL
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/windows/Win64/fmod.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/fmodL.dll)

    add_dependencies (fmod copy_fmod)
    add_dependencies (fmodL copy_fmodL)
endif (WIN32)


if (UNIX)
    set (FMOD_SO_FILE ${CMAKE_CURRENT_SOURCE_DIR}/linux/x86_64/libfmod.so PARENT_SCOPE)

    add_library(fmod SHARED IMPORTED GLOBAL)
    set_target_properties(fmod PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/linux/x86_64/libfmod.so
    )

    add_library(fmodL SHARED IMPORTED GLOBAL)
    set_target_properties(fmodL PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/linux/x86_64/libfmodL.so
    )

    target_include_directories(fmod INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/FMOD)
    target_include_directories(fmodL INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/FMOD)
endif (UNIX)