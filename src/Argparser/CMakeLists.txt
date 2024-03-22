
# List all source files
file(GLOB ARGPARSER_SRCS "*.cpp")

# Create a library
add_library(ArgparserLib ${ARGPARSER_SRCS})
target_include_directories(ArgparserLib 
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/Basis
)