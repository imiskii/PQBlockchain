
# Ledger
file(GLOB LEDGER_SRCS "*.cpp")

add_library(LedgerLib ${LEDGER_SRCS})
target_link_libraries(LedgerLib BasisLib CommonLib HashManagerLib SerLib SignerLib)
target_include_directories(LedgerLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)
