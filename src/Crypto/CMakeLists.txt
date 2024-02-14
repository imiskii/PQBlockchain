
# Hash Manager
add_library(HashManagerLib HashManager.cpp)
target_link_libraries(HashManagerLib CommonLib cryptopp)
target_include_directories(HashManagerLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)

# Signer
add_library(SignerLib Signer.cpp)
target_link_libraries(SignerLib CommonLib cryptopp falcon_1024)
target_include_directories(SignerLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)

