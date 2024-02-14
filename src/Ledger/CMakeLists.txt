
# Ledger
file(GLOB LEDGER_SRCS "*.cpp")

add_library(LedgerLib ${LEDGER_SRCS})
target_link_libraries(LedgerLib CommonLib HashManagerLib)
target_include_directories(LedgerLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)
