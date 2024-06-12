
##
# PQ clean library
# |- Falcon-1024
# |- Kyber-1024
add_subdirectory(PQClean)

##
# Google test library
add_subdirectory(googletest)

##
# spdlog
# use: spdlog
add_subdirectory(spdlog)

##
# Crypto++ library (with modern cmake)
# use: cryptopp
add_subdirectory(cryptopp-cmake)

##
# LevelDB
# use: leveldb
set(LEVELDB_BUILD_TESTS OFF)
set(LEVELDB_BUILD_BENCHMARKS OFF)
add_subdirectory(LevelDB)

##
# nlohmann/json
# use: nlohmann_json::nlohmann_json
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
add_subdirectory(json)
