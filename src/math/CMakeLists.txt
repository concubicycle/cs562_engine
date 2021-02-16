set (MATH_SOURCE_FILES
        
        cross_product_matrix.cpp)

add_library(math ${MATH_SOURCE_FILES})
target_include_directories(math PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(math PUBLIC
    eigen)
