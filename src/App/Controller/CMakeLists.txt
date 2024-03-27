
## Controller
file(GLOB CONTROLLER_SRCS "*.cpp")

add_library(ControllerLib ${CONTROLLER_SRCS})
target_link_libraries(ControllerLib BasisLib InterfaceLib ModelLib)
target_include_directories(ControllerLib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)