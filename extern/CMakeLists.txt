
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
# RocksDB and gflags

ExternalProject_Add(
    gflags
    PREFIX gflags
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/gflags
    CMAKE_ARGS -DBUILD_SHARED_LIBS=ON
)

ExternalProject_Add(
    RocksDB
    PREFIX RocksDB
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/extern/RocksDB
)