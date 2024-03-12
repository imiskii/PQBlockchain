

add_library(CommonLib Blob.cpp HexBase.cpp)
target_link_libraries(CommonLib cryptopp)
target_include_directories(CommonLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)

add_library(SerLib Serialize.cpp)
target_include_directories(SerLib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)