set (PLATFORM_SOURCE_FILES
    sleeper.cpp
)

add_library(platform STATIC ${PLATFORM_SOURCE_FILES})
target_include_directories(platform PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(platform PUBLIC
            $<$<BOOL:${WIN32}>:xinput winmm>)