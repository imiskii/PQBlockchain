

file(GLOB DILITHIUM5_SRCS "*.c" "*.S")
add_library(dilithium5 STATIC ${DILITHIUM5_SRCS})
target_link_libraries(dilithium5 PRIVATE pqclean_common pqclean_keccak4x)
set_target_properties(dilithium5 PROPERTIES C_STANDARD 99)
target_compile_options(dilithium5 PRIVATE -O3 -mpopcnt -mavx2)
target_include_directories(dilithium5 PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(dilithium5 
PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common
PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common/keccak4x
)