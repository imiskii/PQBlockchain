# Root CMakeList.txt

cmake_minimum_required(VERSION 3.28)

# Project name
project(PQBlockchain
        VERSION 0.1
        DESCRIPTION "Post-quantum Blockchain"
        LANGUAGES C CXX
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


# Debug flag
option(ENABLE_DEBUG "Enable debug flags" OFF)
if(ENABLE_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -g")
endif()


# include external libraries
# find_package(Curses REQUIRED)

ExternalProject_Add(
    RocksDB
    PREFIX ${CMAKE_BINARY_DIR}/extern/RocksDB
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/RocksDB
    CONFIGURE_COMMAND ""
    BUILD_IN_SOURCE true
    BUILD_COMMAND make static_lib
    INSTALL_COMMAND ""
)

set(RocksDB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/extern/RocksDB/include)
set(RocksDB_LIB_DIR ${CMAKE_SOURCE_DIR}/extern/RocksDB)
add_library(RocksDBLib STATIC IMPORTED)
set_target_properties(RocksDBLib PROPERTIES IMPORTED_LOCATION ${RocksDB_LIB_DIR}/librocksdb.a)


# Include subdirectiories
add_subdirectory(src)
add_subdirectory(tests)

add_subdirectory(extern)

