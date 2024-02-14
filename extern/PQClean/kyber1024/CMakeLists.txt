
file(GLOB KYBER_SRCS "*.c")
add_library(kyber_1024 STATIC ${KYBER_SRCS})
target_link_libraries(kyber_1024 PRIVATE pqclean_common)
set_target_properties(kyber_1024 PROPERTIES C_STANDARD 99)
target_compile_options(kyber_1024 PRIVATE -O3)
target_include_directories(kyber_1024 PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(kyber_1024 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)