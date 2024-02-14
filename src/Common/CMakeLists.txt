

# Blob
file(GLOB COMMON_SRCS "*.cpp")

add_library(CommonLib ${COMMON_SRCS})
target_link_libraries(CommonLib cryptopp)
target_include_directories(CommonLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)