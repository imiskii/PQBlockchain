
## MAIN.cpp

# Create an executable
add_executable(main main.cpp)

# Link other libraries
target_link_libraries(main PRIVATE ArgparserLib PQBlockchainExceptionsLib)
target_include_directories(main 
    PRIVATE ${CMAKE_SOURCE_DIR}/src/Argparser
    PRIVATE ${CMAKE_SOURCE_DIR}/src
)


## KEM.cpp
add_executable(kem kem.cpp)
target_link_libraries(kem PRIVATE kyber_1024)
target_include_directories(kem 
    PRIVATE ${CMAKE_SOURCE_DIR}/extern/PQClean/kyber1024
)


## SIG.cpp
add_executable(sig sig.cpp)
target_link_libraries(sig PRIVATE falcon_1024)
target_include_directories(sig 
    PRIVATE ${CMAKE_SOURCE_DIR}/extern/PQClean/falcon1024
)