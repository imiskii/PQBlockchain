
file(GLOB COMMON_SRCS "*.c")
add_library(pqclean_common STATIC ${COMMON_SRCS})
set_target_properties(pqclean_common PROPERTIES C_STANDARD 99)
target_compile_options(pqclean_common PRIVATE -O3)
target_include_directories(pqclean_common PUBLIC ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(keccak4x)