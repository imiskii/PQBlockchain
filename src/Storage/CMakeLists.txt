

# Storage
file(GLOB STORAGE_SRCS "*.cpp")

add_library(StorageLib ${STORAGE_SRCS})
target_link_libraries(StorageLib leveldb CommonLib LedgerLib SerLib SignerLib HashManagerLib MerkleTreeHashLib)
target_include_directories(StorageLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)
