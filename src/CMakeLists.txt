
add_subdirectory(App)
add_subdirectory(Argparser)
add_subdirectory(Common)
add_subdirectory(Ledger)
add_subdirectory(Crypto)
add_subdirectory(Nodes)
add_subdirectory(Storage)
add_subdirectory(Network)
add_subdirectory(Consensus)
add_subdirectory(Wallet)

include_directories(${CMAKE_CURRENT_LIST_DIR}/Basis)