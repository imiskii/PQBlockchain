
add_subdirectory(View)
add_subdirectory(Controller)
add_subdirectory(Model)

## MAIN.cpp

# Create an executable
add_executable(main main.cpp)

# Link other libraries
target_link_libraries(main PRIVATE ArgparserLib InterfaceLib ControllerLib)


## KEM.cpp
add_executable(kem kem.cpp)
target_link_libraries(kem PRIVATE kyber_1024)


## SIG.cpp
add_executable(sig sig.cpp)
target_link_libraries(sig PRIVATE falcon_1024 CommonLib)


## TMP.cpp
add_executable(tmp tmp.cpp)
target_link_libraries(tmp PRIVATE CommonLib HashManagerLib)

