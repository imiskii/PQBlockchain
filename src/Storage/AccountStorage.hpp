/**
 * @file AccountStorage.hpp
 * @author Michal Ľaš
 * @brief LevelDB databese for storing account balances of blockchain's nodes
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


class AccountBalancesStorage{
public:
    
    AccountBalancesStorage();
    ~AccountBalancesStorage();

    /**
     * @brief Open LevelDB database
     * @exception If database fails to open
     * 
     */
    void openDatabase();

    /**
     * @brief Get the account balance
     * 
     * @param accountID unique account identifier
     * @return cash availble resources (cryptocurrency)
     * @exception if LevelDB get operation fails
     */
    cash getAccountBalance(byte64_t& accountID);

    /**
     * @brief Updates balance of given account
     * 
     * @param accountID unique account identifier
     * @param newAmount amount to update the account balance
     * @exception if LevelDB put operation fails
     */
    void updateBalance(byte64_t& accountID, cash newAmount);

    /**
     * @brief Update all account balances base on given block transactions
     * 
     * @param block block with transactions
     * @todo MAKE IT NOT FROM BLOCK, BUT FROM TRANSACTION SET
     */
    void updateBalancesByBlock(unsigned& block);

private:
    leveldb::Options databaseOptions;
    leveldb::DB* db; ///< Instance of RocksDB database
};


} // namespace PQB


/* END OF FILE */