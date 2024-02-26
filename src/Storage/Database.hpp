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
#include "leveldb/db.h"

#include "Blob.hpp"
#include "Block.hpp"
#include "Transaction.hpp"


namespace PQB
{

class AccountBalancesStorage{
public:
    
    AccountBalancesStorage();
    ~AccountBalancesStorage();

    void openDatabase();

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
    leveldb::Options databaseOptions;
    leveldb::DB* db; ///< Instance of RocksDB database
};


class BlocksStorage{
public:

    BlocksStorage(leveldb::DB* database) : db(database) {}

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
    leveldb::DB* db; ///< Instance of RocksDB database
};


class AccountAddressStorage{
public:

    AccountAddressStorage(leveldb::DB* database) : db(database) {}

    void getAddress(byte64_t& accountID);

    void updateAddress(byte64_t& accountID, unsigned newAddress);
    
private:
    leveldb::DB* db; ///< Instance of RocksDB database
};



} // namespace PQB





/* END OF FILE */