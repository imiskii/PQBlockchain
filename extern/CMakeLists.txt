
##
# PQ clean library
# |- Falcon-1024
# |- Kyber-1024
add_subdirectory(PQClean)

##
# Google test library
add_subdirectory(googletest)

##
# Crypto++ library (with modern cmake)
add_subdirectory(cryptopp-cmake)


##
# RocksDB
#add_subdirectory(gflags)

ExternalProject_Add(
    gflags
    PREFIX ${CMAKE_SOURCE_DIR}/extern/gflags
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/gflags
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

#set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR}/gflags/build)
#add_subdirectory(RocksDB)

ExternalProject_Add(
    RocksDB
    PREFIX ${CMAKE_SOURCE_DIR}/extern/RocksDB
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/RocksDB
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)