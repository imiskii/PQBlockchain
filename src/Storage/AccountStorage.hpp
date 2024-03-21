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

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <cstring>
#include "leveldb/db.h"
#include "leveldb/slice.h"
#include "leveldb/write_batch.h"
#include "PQBtypedefs.hpp"
#include "Account.hpp"
#include "Blob.hpp"
#include "Transaction.hpp"
#include "Serialize.hpp"
#include "Signer.hpp"
#include "HashManager.hpp"
#include "MerkleRootCompute.hpp"

namespace PQB{


class AccountBalanceStorage{
public:

    AccountBalanceStorage();
    ~AccountBalanceStorage();

    /**
     * @brief Open LevelDB database for account balances
     * @exception If database fails to open
     * 
     */
    void Open();

    /**
     * @brief Query a BalanceData structure from the database
     * 
     * @param walletID ID of account to query
     * @param acc [out] output balance data to Account object
     * @return true if wallet was found
     * @return false if wallet was not found or if database Get operation fails
     */
    bool getBalance(const byte64_t &walletID, AccountBalance &acc) const;

    /**
     * @brief Put a BalanceData structure to the database.
     * If wallet ID is in the database it will replace the current value,
     * if wallet ID is not in the database it will put new value into the database under walletID key
     * 
     * @param walletID ID of account to which assign the data
     * @param acc Account data to put into the database
     * @return true if operation was successful
     * @return false if operation fails
     */
    bool setBalance(const byte64_t &walletID, AccountBalance &acc);

    /**
     * @brief Update all account balances base on given block transactions set
     * 
     * @param txSet set of transactions to update
     * 
     * @exception If sender or receiver wallet ID of any transaction is not in the database or if write to the database failed
     */
    void setBalancesByTxSet(std::set<TransactionPtr, TransactionPtrComparator> &txSet);

    /**
     * @brief Calculate merkle tree root hash of all accounts (account balances) in the database
     * 
     * @return byte64_t root hash of account balances
     */
    byte64_t getAccountsMerkleRootHash();

protected:
    leveldb::DB* db; ///< Instance of LevelDB database for Accounts

private:
    struct AccountDifference{
        byte64_t *id;
        signed long balanceDiff;
        uint32_t txSequence;
    };

    leveldb::Options databaseOptions;

    std::unordered_map<std::string, AccountDifference> countAccountDifferencesByTxSet(std::set<TransactionPtr, TransactionPtrComparator> &txSet);

};


/// @brief Management of storage for accounts network addresses
class AccountAddressStorage{
public:

    AccountAddressStorage();
    ~AccountAddressStorage();

    /**
     * @brief Open LevelDB database for account addresses
     * @exception If database fails to open
     * 
     */
    void Open();

    /**
     * @brief Query an account addresses from the database
     * 
     * @param walletID ID of account to query
     * @param acc [out] output addresses to Account object
     * @return true if wallet was found
     * @return false if wallet was not found
     */
    bool getAddresses(const byte64_t &walletID, AccountAddress &acc) const;

    /**
     * @brief Put an account addresses to the database.
     * If wallet ID is in the database it will replace the current value,
     * if wallet ID is not in the database it will put new value into the database under walletID key
     * 
     * @param walletID ID of account to which assign the data
     * @param acc Account addresses to put into the database
     * @return true if operation was successful
     * @return false if operation fails
     */
    bool setAddresses(const byte64_t &walletID, AccountAddress &acc);


protected:
    leveldb::DB* db; ///< Instance of LevelDB database for Accounts

private:
    leveldb::Options databaseOptions;

};


/**
 * @brief This class manages 2 LevelDB databases. First for Account balances, publick keys and sequence numbers of the transactions, and
 * second for Account network addresses. These data are splited like this because ConnectionManager use to access just addresses and Consensus need
 * just public key of an account, balance and sequence number. Only downside is that when sending account information to other these information have to be merged.
 */
class AccountStorage{
public:
    
    AccountStorage();
    ~AccountStorage();

    /**
     * @brief Open LevelDB database for accounts and addresses
     * @exception If database fails to open
     * 
     */
    void openDatabases();

    /**
     * @brief Query an account from the database
     * 
     * @param walletID ID of an account
     * @param acc [out] Queried Account
     * @return true if operation was successful
     * @return false if operation fails or wallet was not found
     */
    bool getAccount(const byte64_t &walletID, Account &acc);

    /**
     * @brief Set the Account object
     * 
     * @param walletID ID of an account
     * @param acc Account to put into database
     * @return true if operation was successful
     * @return false if operation fails
     */
    bool setAccount(const byte64_t &walletID, Account &acc);


    AccountBalanceStorage *blncDB; ///< balance database
    AccountAddressStorage *addrDB; ///< address database
};


} // namespace PQB


/* END OF FILE */