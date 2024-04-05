/**
 * @file constatnts.hpp
 * @author Michal Ľaš
 * @brief global constants for PQB
 * @date 2024-02-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace PQB{

    extern const std::string_view BLOCKS_DATABASE_PATH;
    extern const std::string_view ACCOUNTS_DATABASE_PATH;
    extern const std::string_view ADDRESS_DATABASE_PATH;

    extern const std::string_view CONFIGURATION_FILE;
    extern const std::string_view LOG_FILE;

    extern const size_t MAX_BLOCK_SIZE;

    extern const size_t MAX_MESSAGE_SIZE;
    extern const size_t PQB_SERVER_PORT;
    extern const size_t MAX_SERVER_QUEUE;

    extern const uint32_t MSG_VERSION;
    extern const uint32_t TX_VERSION;

    extern const std::string_view GENESIS_BLOCK_HASH;
}


/* END OF FILE */