

# NetLib
file(GLOB NET_SRCS "*.cpp")

add_library(NetLib ${NET_SRCS})
target_link_libraries(NetLib BasisLib CommonLib HashManagerLib LedgerLib StorageLib AccountLib SerLib WalletLib ConsensusLib)
target_include_directories(NetLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)
