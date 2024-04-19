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
    /// @brief SHA-512 hash of the empty string, It is used for message checking if message was well parsed and as a padding for empty
    /// hash values for exmaple in block's transaction hash etc., if they are not calculated yet or they are just empty
    constexpr std::string_view EMPTY_STRING_HASH = "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e";

    /// @brief Max si of the message sended/received with a socket
    constexpr size_t MAX_MESSAGE_SIZE = 1400;
    /// @brief Network port for PQB server application
    constexpr size_t PQB_SERVER_PORT = 8330;
    /// @brief Maximal number of incoming connection requests in queue for accepting. This may be really important when createing lot of connections at once!!!
    constexpr size_t MAX_SERVER_QUEUE = 30;

    /// @brief Message Version
    constexpr uint32_t MSG_VERSION = 1;
    /// @brief Current transaction version
    constexpr uint32_t TX_VERSION = 1;

    /// ID/hash of the genesis block
    constexpr std::string_view GENESIS_BLOCK_HASH = "B40FA957FE22271545BF8082FC798C8D0E3C9788DC18793755B39035CD5B142DFC04C316C3256C48D0EDCFEE62779B5C74A0BFA2BF4F680BFD2C9500FFA118ED";
}

/* END OF FILE */