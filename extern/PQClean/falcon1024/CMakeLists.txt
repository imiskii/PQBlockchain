
file(GLOB FALCON1024_SRCS "*.c")
add_library(falcon_1024 STATIC ${FALCON1024_SRCS})
target_link_libraries(falcon_1024 PRIVATE pqclean_common)
set_target_properties(falcon_1024 PROPERTIES C_STANDARD 99)
target_compile_options(falcon_1024 PRIVATE -O2 -mavx2)
target_include_directories(falcon_1024 PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_include_directories(falcon_1024 PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)