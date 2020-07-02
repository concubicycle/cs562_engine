add_executable(irrmapgen "irrmapgen_main.cpp" "irrmapgen.cpp" "spherical_harmonic_approximator.cpp")
 
target_compile_features(irrmapgen PUBLIC cxx_std_17)
target_link_libraries(irrmapgen asset eigen)