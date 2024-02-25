
## Interface
file(GLOB VIEW_SRCS "*.cpp")

add_library(InterfaceLib ${VIEW_SRCS})
target_link_libraries(InterfaceLib ) # ${CURSES_LIBRARIES}
target_include_directories(InterfaceLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    #PRIVATE ${CURSES_INCLUDE_DIR}
)