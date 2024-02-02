
# List all source files
file(GLOB ARGPARSER_SRCS "*.cpp")

# Create a library
add_library(ArgparserLib ${ARGPARSER_SRCS})
target_include_directories(ArgparserLib PRIVATE ${CMAKE_CURRENT_LIST_DIR})