
## Model
file(GLOB MODEL_SRCS "*.cpp")

add_library(ModelLib ${MODEL_SRCS})
target_link_libraries(ModelLib BasisLib CommonLib StorageLib WalletLib NetLib ConsensusLib)
target_include_directories(ModelLib
    PUBLIC ${CMAKE_CURRENT_LIST_DIR}
)