

file(GLOB DILITHIUM2_SRCS "*.c" "*.S")
add_library(dilithium2 STATIC ${DILITHIUM2_SRCS})
target_link_libraries(dilithium2 PRIVATE pqclean_common pqclean_keccak4x)
set_target_properties(dilithium2 PROPERTIES C_STANDARD 99)
target_compile_options(dilithium2 PRIVATE -O3 -mpopcnt -mavx2)
target_include_directories(dilithium2 PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(dilithium2 
PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common
PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common/keccak4x
)