

add_library(pqclean_keccak4x STATIC KeccakP-1600-times4-SIMD256.c)
set_target_properties(pqclean_keccak4x PROPERTIES C_STANDARD 99)
target_compile_options(pqclean_keccak4x PRIVATE -O3 -mavx2)
target_include_directories(pqclean_keccak4x PUBLIC ${CMAKE_CURRENT_LIST_DIR})
