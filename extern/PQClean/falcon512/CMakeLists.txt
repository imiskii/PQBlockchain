
file(GLOB FALCON512_SRCS "*.c")
add_library(falcon_512 STATIC ${FALCON512_SRCS})
target_link_libraries(falcon_512 PRIVATE pqclean_common)
set_target_properties(falcon_512 PROPERTIES C_STANDARD 99)
target_compile_options(falcon_512 PRIVATE -O2 -mavx2)
target_include_directories(falcon_512 PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(falcon_512 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)