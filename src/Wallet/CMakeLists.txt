
# Wallet
file(GLOB WALLET_SRCS "*.cpp")

add_library(WalletLib ${WALLET_SRCS})
target_link_libraries(WalletLib BasisLib nlohmann_json::nlohmann_json CommonLib LedgerLib SignerLib HashManagerLib)
target_include_directories(WalletLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)