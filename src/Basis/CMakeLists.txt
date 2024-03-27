

file(GLOB BASIS_SRCS "*.cpp")

add_library(BasisLib ${BASIS_SRCS})
target_include_directories(BasisLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)