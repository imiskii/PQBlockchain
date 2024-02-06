
add_subdirectory(App)
add_subdirectory(Argparser)
add_subdirectory(Nodes)


add_library(PQBlockchainExceptionsLib INTERFACE)
target_sources(PQBlockchainExceptionsLib INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/PQBlockchainExceptions.hpp
)
target_include_directories(PQBlockchainExceptionsLib INTERFACE ${CMAKE_CURRENT_LIST_DIR})