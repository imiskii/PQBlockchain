/**
 * @file Database.hpp
 * @author Michal Ľaš
 * @brief 
 * @date 2024-02-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <vector>
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>

#include <PQBconstatnts.hpp>
#include "Blob.hpp"
#include "Block.hpp"
#include "Transaction.hpp"


namespace PQB
{


/// @brief RocksDB for storing PQB data
class Database{
public:
    Database();
    ~Database();

    /**
     * @brief Open the database
     * @exception if there was an error during opening the database
     * 
     */
    void openDatabase();

    /**
     * @brief Close the database
     * @exception if there was an error during closing the database family handles
     * 
     */
    void closeDatabase();

    rocksdb::DB* getDatabase(){
        return db;
    }

    // Handles for the database column families
    rocksdb::ColumnFamilyHandle* blockStorageHandle;
    rocksdb::ColumnFamilyHandle* accountStorageHandle;
    rocksdb::ColumnFamilyHandle* addressStorageHandle;

private:
    const char* databasePath = "../../../tmp/rocksdb"; ///< location of database
    rocksdb::Options databaseInitializeOptions;
    rocksdb::DB* db; ///< Instance of RocksDB database
    std::vector<rocksdb::ColumnFamilyDescriptor> columnFamilies; ///< logical partitions of the database
};


class AccountBalancesStorage{
public:
    
    AccountBalancesStorage(rocksdb::DB* database, rocksdb::ColumnFamilyHandle* accountBalancesStorageHandler) : db(database), CFHandler(accountBalancesStorageHandler) {}

    /**
     * @brief Get the account balance
     * 
     * @param accountID unique account identifier
     * @return cash availble resources (cryptocurrency)
     * @exception if get operation fails
     */
    cash getAccountBalance(byte64_t& accountID);

    /**
     * @brief Updates balance of given account
     * 
     * @param accountID unique account identifier
     * @param newAmount amount to update the account balance
     * @exception if put operation fails
     */
    void updateBalance(byte64_t& accountID, cash newAmount);

    /**
     * @brief Update all account balances base on given block transactions
     * 
     * @param block block with transactions
     */
    void updateBalancesByBlock(Block& block);

private:
    rocksdb::DB* db; ///< Instance of RocksDB database
    rocksdb::ColumnFamilyHandle* CFHandler;
};


class BlocksStorage{
public:

    BlocksStorage(rocksdb::DB* database, rocksdb::ColumnFamilyHandle* blocksStorageHandler) : db(database), CFHandler(blocksStorageHandler) {}

    /**
     * @brief Get the block data from database
     * 
     * @param blockHeight block height (number representiong position of the block in the blockchain)
     * @return Block* pointer to block's data
     */
    Block* getBlock(size_t blockHeight);

    /**
     * @brief Put block into database
     * 
     * @param block block data that will be inserted in database
     */
    void saveBlock(Block& block);

private:
    rocksdb::DB* db; ///< Instance of RocksDB database
    rocksdb::ColumnFamilyHandle* CFHandler; ///< blocks column family
};


class AccountAddressStorage{
public:

    AccountAddressStorage(rocksdb::DB* database, rocksdb::ColumnFamilyHandle* addressStorageHandler) : db(database), CFHandler(addressStorageHandler) {}

    void getAddress(byte64_t& accountID);

    void updateAddress(byte64_t& accountID, unsigned newAddress);
    
private:
    rocksdb::DB* db; ///< Instance of RocksDB database
    rocksdb::ColumnFamilyHandle* CFHandler; ///< blocks column family
};



} // namespace PQB





/* END OF FILE */