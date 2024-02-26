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


namespace PQB{

    /// @brief Location on the LevelDB databse where are stored blockchain blocks
    const char* BLOCKS_DATABASE_PATH = "tmp/blocksStorage";
    /// @brief Location on the LevelDB databse where are stored balances and public keys of individual accounts
    const char* ACCOUNTS_DATABASE_PATH = "tmp/accountStorage";
    /// @brief Location on the LevelDB databse where are stored IPv4, IPv6 address and aliases of other nodes (peers)
    const char* ADDRESS_DATABASE_PATH = "tmp/addressStorage";
    
}


/* END OF FILE */