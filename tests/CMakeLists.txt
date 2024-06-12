
# Create executeble for tests
#add_executable(Scitanie sucet.cpp)
#add_test(Test Scitanie 100 100) / add_test(NAME Test COMMAND Scitanie)
#add_tests_properties(Test PROPERTIES PASS_REGULAR_EXPRESSION "200")

# add_test(NAME <název> COMMAND <přı́kaz> [args])
# • Vytvořı́ cı́l s názvem <název> pro testovánı́
# • Pokud je <přı́kaz> názvem spustitelného cı́le je jı́m nahrazen
# • Test lze spustit pomocı́ ctest -R <název>“

# set_tests_properties(<název> PROPERTIES <param>)
# • Umožňuje nastavit parametry testu
# • PASS REGULAR EXPRESSION – výstup musı́ odpovı́dat R. V.
# • TIMEOUT – časový limit na provedenı́ testu
# • COST – specifikuje pořadı́ testů



## 
# Macro for adding new tests
# 
# Examples:
# + package_add_test(Example example.cpp "ArgParser;AccountLib" "${PROJECT_DIR}")
# + package_add_test(Example example.cpp "" "${PROJECT_DIR}")                   // no libraries
# + package_add_test(Example example1.cpp example2.cpp "" "${PROJECT_DIR}")     // multiple sources
macro(package_add_test TESTNAME FILES LIBRARIES TEST_WORKING_DIRECTORY)
    add_executable(${TESTNAME} ${FILES})
    message(STATUS "${TESTNAME} - LIBRARIES: ${LIBRARIES}")
    target_link_libraries(${TESTNAME} gtest gmock gtest_main ${LIBRARIES})
    gtest_discover_tests(${TESTNAME}
        WORKING_DIRECTORY ${TEST_WORKING_DIRECTORY}
        PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${TEST_WORKING_DIRECTORY}"
    )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
endmacro()




if(BUILD_TESTING)
    enable_testing()
    include(GoogleTest)

    # Signer
    package_add_test(Falcon1024Sign Signer/falcon1024.cpp "SignerLib;CommonLib" "${PROJECT_DIR}")
    package_add_test(Falcon512Sign Signer/falcon512.cpp "SignerLib;CommonLib" "${PROJECT_DIR}")
    package_add_test(Dilithium5Sign Signer/dilithium5.cpp "SignerLib;CommonLib" "${PROJECT_DIR}")
    package_add_test(Dilithium3Sign Signer/dilithium3.cpp "SignerLib;CommonLib" "${PROJECT_DIR}")
    package_add_test(Dilithium2Sign Signer/dilithium2.cpp "SignerLib;CommonLib" "${PROJECT_DIR}")
    package_add_test(Ed25519Sign Signer/ed25519.cpp "SignerLib;CommonLib" "${PROJECT_DIR}")
    package_add_test(ECDSASign Signer/ecdsa.cpp "SignerLib;CommonLib" "${PROJECT_DIR}")
    
    # Common
    package_add_test(Blob Common/Blob.cpp "CommonLib" "${PROJECT_DIR}")
    package_add_test(HexBase Common/HexBase.cpp "CommonLib" "${PROJECT_DIR}")
    package_add_test(Serialize Common/Serialize.cpp "CommonLib;BasisLib" "${PROJECT_DIR}")
    # Hash Manager
    package_add_test(HashManager HashManager/HashManager.cpp "HashManagerLib" "${PROJECT_DIR}")

    # Merkle Tree
    package_add_test(MerkleRoot MerkleTree/MerkleRootCompute.cpp "MerkleTreeHashLib" "${PROJECT_DIR}")

    # Structures
    package_add_test(Transaction Structures/Transaction.cpp "LedgerLib" "${PROJECT_DIR}")
    package_add_test(Block Structures/Block.cpp "LedgerLib" "${PROJECT_DIR}")
    package_add_test(Account Structures/Account.cpp "AccountLib" "${PROJECT_DIR}")
    package_add_test(Proposal Structures/Proposal.cpp "ConsensusLib" "${PROJECT_DIR}")

    # Storage
    package_add_test(AccountStorage Storage/AccountStorage.cpp "StorageLib" "${PROJECT_SOURCE_DIR}")
    package_add_test(BlockStorage Storage/BlocksStorage.cpp "StorageLib" "${PROJECT_SOURCE_DIR}")

    # Wallet
    package_add_test(Wallet Wallet/Wallet.cpp "WalletLib;BasisLib" "${PROJECT_SOURCE_DIR}")

endif()

