
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
set(WITH_GFLAGS OFF)
set(WITH_TESTS OFF)
set(WITH_BENCHMARK_TOOLS OFF)
set(WITH_CORE_TOOLS OFF)
set(WITH_TOOLS OFF)
set(WITH_ALL_TESTS OFF)
add_subdirectory(RocksDB)
