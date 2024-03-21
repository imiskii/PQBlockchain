
# Consensus
add_library(ConsensusLib Consensus.cpp)
target_link_libraries(ConsensusLib HashManagerLib CommonLib LedgerLib)
target_include_directories(ConsensusLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Basis
)

# Merkle tree
add_library(MerkleTreeHashLib MerkleRootCompute.cpp)
target_link_libraries(MerkleTreeHashLib HashManagerLib CommonLib LedgerLib)
target_include_directories(MerkleTreeHashLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Basis
)