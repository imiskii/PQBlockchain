
file(GLOB CONSENSUS_SRCS "*.cpp")

add_library(ConsensusLib ${CONSENSUS_SRCS})
target_link_libraries(ConsensusLib HashManagerLib CommonLib LedgerLib)
target_include_directories(ConsensusLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}/Basis
)