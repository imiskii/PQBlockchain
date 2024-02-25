
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
# LevelDB
set(LEVELDB_BUILD_TESTS OFF)
set(LEVELDB_BUILD_BENCHMARKS OFF)
add_subdirectory(LevelDB)
