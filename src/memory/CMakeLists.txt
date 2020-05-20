set(MEMORY_SOURCE_LIST
    ${PROJECT_SOURCE_DIR}/src/memory/alignment.cpp
    ${PROJECT_SOURCE_DIR}/src/memory/pool_allocator.cpp
    ${PROJECT_SOURCE_DIR}/src/memory/pool_memory_block.cpp)

add_library(memory ${MEMORY_SOURCE_LIST})
target_include_directories(memory PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_compile_features(memory PUBLIC cxx_std_17)