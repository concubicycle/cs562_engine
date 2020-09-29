set (MATH_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/math/quat.cpp)

add_library(math ${MATH_SOURCE_FILES})
target_include_directories(math PUBLIC ${PROJECT_SOURCE_DIR}/include)