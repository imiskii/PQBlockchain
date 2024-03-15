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

namespace PQB{

    /// @brief Location on the LevelDB databse where are stored blockchain blocks
    inline const char* BLOCKS_DATABASE_PATH = "tmp/blocksStorage";
    /// @brief Location on the LevelDB databse where are stored public keys, balances, transaction sequence num., and network addresses of individual accounts
    inline const char* ACCOUNTS_DATABASE_PATH = "tmp/balanceStorage";
    /// @brief Location on the LevelDB databse where are stored IPv4, IPv6 address and aliases of other nodes (peers)
    inline const char* ADDRESS_DATABASE_PATH = "tmp/addressStorage";

    /// @brief Maximal size of a block in bytes (1MB)
    inline const size_t MAX_BLOCK_SIZE = 1048576;

    /// @brief Max si of the message sended/received with a socket
    inline const size_t MAX_MESSAGE_SIZE = 1400;
    /// @brief Network port for PQB server application
    inline const size_t PQB_SERVER_PORT = 8330;
    /// @brief Maximal number of incoming connection requests in queue for accepting 
    inline const size_t MAX_SERVER_QUEUE = 6;
}


/* END OF FILE */