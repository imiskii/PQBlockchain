/**
 * @file AddressStorage.hpp
 * @author Michal Ľaš
 * @brief LevelDB databese for storing nodes IPv4, IPv6 addresses and aliases
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "leveldb/db.h"
#include "leveldb/slice.h"
#include "Blob.hpp"

namespace PQB{
    

class AccountAddressStorage{
public:

    AccountAddressStorage();
    ~AccountAddressStorage();

    /**
     * @brief Open LevelDB database
     * @exception If database fails to open
     * 
     */
    void openDatabase();

    void getAddress(byte64_t& accountID);

    void updateAddress(byte64_t& accountID, unsigned newAddress);
    
private:
    leveldb::Options databaseOptions;
    leveldb::DB* db; ///< Instance of RocksDB database
};


} // namespace PQB

/* END OF FILE */