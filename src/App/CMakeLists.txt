
## MAIN.cpp

# Create an executable
add_executable(main main.cpp)

# Link other libraries
target_link_libraries(main PRIVATE ArgparserLib CommonLib InterfaceLib SignerLib)


## KEM.cpp
add_executable(kem kem.cpp)
target_link_libraries(kem PRIVATE kyber_1024)



## SIG.cpp
add_executable(sig sig.cpp)
target_link_libraries(sig PRIVATE falcon_1024 CommonLib)


## Interface
add_library(InterfaceLib Interface.cpp)
#target_link_libraries(InterfaceLib cryptopp)
target_include_directories(InterfaceLib PUBLIC ${CMAKE_CURRENT_LIST_DIR})