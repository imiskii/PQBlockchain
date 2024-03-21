

# Account
add_library(AccountLib Account.cpp)
target_link_libraries(AccountLib CommonLib SerLib SignerLib HashManagerLib)
target_include_directories(AccountLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)

# Storage
add_library(StorageLib AccountStorage.cpp BlocksStorage.cpp)
target_link_libraries(StorageLib leveldb CommonLib LedgerLib SerLib SignerLib HashManagerLib MerkleTreeHashLib AccountLib)
target_include_directories(StorageLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)