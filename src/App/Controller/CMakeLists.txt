
## Controller
file(GLOB CONTROLLER_SRCS "*.cpp")

add_library(ControllerLib ${CONTROLLER_SRCS})
target_link_libraries(ControllerLib InterfaceLib)
target_include_directories(ControllerLib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)