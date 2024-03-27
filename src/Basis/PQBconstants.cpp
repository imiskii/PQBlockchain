/**
 * @file PQBconstants.cpp
 * @author Michal Ľaš
 * @brief global constants for PQB
 * @date 2024-02-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "PQBconstants.hpp"

namespace PQB{

    /// @brief Location on the LevelDB databse where are stored blockchain blocks
    constexpr std::string_view BLOCKS_DATABASE_PATH = "tmp/blocksStorage";
    /// @brief Location on the LevelDB databse where are stored public keys, balances, transaction sequence num., and network addresses of individual accounts
    constexpr std::string_view ACCOUNTS_DATABASE_PATH = "tmp/balanceStorage";
    /// @brief Location on the LevelDB databse where are stored IPv4, IPv6 address and aliases of other nodes (peers)
    constexpr std::string_view ADDRESS_DATABASE_PATH = "tmp/addressStorage";

    /// @brief Configuration file for PQB application, mainly information about wallet, made transactions, UNL, etc.
    constexpr std::string_view CONFIGURATION_FILE = "tmp/conf.json";
    /// @brief Path to the file for PQB logs
    constexpr std::string_view LOG_FILE = "tmp/log.txt";

    /// @brief Maximal size of a block in bytes (1MB)
    constexpr size_t MAX_BLOCK_SIZE = 1048576;

    /// @brief Max si of the message sended/received with a socket
    constexpr size_t MAX_MESSAGE_SIZE = 1400;
    /// @brief Network port for PQB server application
    constexpr size_t PQB_SERVER_PORT = 8330;
    /// @brief Maximal number of incoming connection requests in queue for accepting 
    constexpr size_t MAX_SERVER_QUEUE = 6;

    /// @brief Message Version
    constexpr uint32_t MSG_VERSION = 1;
    /// @brief Current transaction version
    constexpr uint32_t TX_VERSION = 1;
}

/* END OF FILE */