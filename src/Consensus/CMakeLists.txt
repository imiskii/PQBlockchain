

# Chain
add_library(ChainLib Chain.cpp)
target_link_libraries(ChainLib CommonLib LedgerLib)
target_include_directories(ChainLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

# Consensus
add_library(ConsensusLib Consensus.cpp ConsensusParams.cpp Proposal.cpp)
target_link_libraries(ConsensusLib BasisLib HashManagerLib MerkleTreeHashLib CommonLib LedgerLib ChainLib StorageLib WalletLib AccountLib NetLib)
target_include_directories(ConsensusLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

# Merkle tree
add_library(MerkleTreeHashLib MerkleRootCompute.cpp)
target_link_libraries(MerkleTreeHashLib BasisLib HashManagerLib CommonLib LedgerLib)
target_include_directories(MerkleTreeHashLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)