

# Storage
file(GLOB STORAGE_SRCS "*.cpp")

add_library(StorageLib ${STORAGE_SRCS})
target_link_libraries(StorageLib RocksDBLib CommonLib LedgerLib)
target_include_directories(StorageLib 
    PUBLIC ${RocksDB_INCLUDE_DIR}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)
