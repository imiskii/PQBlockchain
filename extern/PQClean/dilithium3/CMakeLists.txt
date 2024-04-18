

file(GLOB DILITHIUM3_SRCS "*.c" "*.S")
add_library(dilithium3 STATIC ${DILITHIUM3_SRCS})
target_link_libraries(dilithium3 PRIVATE pqclean_common pqclean_keccak4x)
set_target_properties(dilithium3 PROPERTIES C_STANDARD 99)
target_compile_options(dilithium3 PRIVATE -O3 -mpopcnt -mavx2)
target_include_directories(dilithium3 PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(dilithium3 
PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common
PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common/keccak4x
)