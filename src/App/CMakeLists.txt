
add_subdirectory(View)
add_subdirectory(Controller)
add_subdirectory(Model)

## MAIN.cpp

# Create an executable
add_executable(pqb pqb.cpp)
target_link_libraries(pqb PRIVATE ArgparserLib InterfaceLib ControllerLib ModelLib SignerLib CommonLib)


## Configuration generator
add_executable(conf-generator conf-generator.cpp)
target_link_libraries(conf-generator PRIVATE CommonLib SignerLib WalletLib)

## Account Database populator
add_executable(acc-generator acc-generator.cpp)
target_link_libraries(acc-generator PRIVATE CommonLib AccountLib StorageLib SignerLib WalletLib)

## Command Runner
add_executable(commandRunner commandRunner.cpp)