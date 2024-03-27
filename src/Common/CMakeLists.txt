

add_library(CommonLib Blob.cpp HexBase.cpp Log.cpp)
target_link_libraries(CommonLib BasisLib cryptopp spdlog)
target_include_directories(CommonLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)

add_library(SerLib Serialize.cpp)
target_link_libraries(SerLib BasisLib)
target_include_directories(SerLib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)