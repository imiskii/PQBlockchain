# Root CMakeList.txt

cmake_minimum_required(VERSION 3.28)

# Project name
project(PQBlockchain
        VERSION 0.1
        DESCRIPTION "Post-quantum Blockchain"
        LANGUAGES C CXX ASM
)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#Set C standard
set(CMAKE_C_STANDARD 99)


include(ExternalProject)
include(CTest)


# Global compile options
add_compile_options(-Wall -Wextra -Wpedantic)
add_link_options(-pthread)


# Debug flag
option(ENABLE_DEBUG "Enable debug flags" OFF)
if(ENABLE_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -g")
endif()


# Include subdirectiories
add_subdirectory(src)
add_subdirectory(tests)

add_subdirectory(extern)

