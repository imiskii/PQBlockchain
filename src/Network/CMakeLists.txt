

# NetLib
file(GLOB NET_SRCS "*.cpp")

add_library(NetLib ${NET_SRCS})
target_link_libraries(NetLib CommonLib HashManagerLib LedgerLib)
target_include_directories(NetLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)
