
# Hash Manager
add_library(HashManagerLib HashManager.cpp)
target_link_libraries(HashManagerLib BasisLib CommonLib cryptopp)
target_include_directories(HashManagerLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

# Signer
add_library(SignerLib Signer.cpp)
target_link_libraries(SignerLib BasisLib CommonLib cryptopp falcon_1024)
target_include_directories(SignerLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

