set (AUDIO_SOURCE_LIST
        ${PROJECT_SOURCE_DIR}/src/audio/audio_system.cpp
        ${PROJECT_SOURCE_DIR}/src/audio/audio_listener.cpp
        ${PROJECT_SOURCE_DIR}/src/audio/audio_emitter.cpp 
        ${PROJECT_SOURCE_DIR}/src/audio/music_player.cpp)

add_library(audio ${AUDIO_SOURCE_LIST})
target_include_directories(audio PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(audio PUBLIC cxx_std_17)
target_link_libraries(audio PUBLIC collisions ecs transforms asset fmod eigen)


