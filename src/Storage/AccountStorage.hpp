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
#include "Blob.hpp"
#include "Transaction.hpp"
#include "Serialize.hpp"
#include "Signer.hpp"
#include "HashManager.hpp"
#include "MerkleRootCompute.hpp"

namespace PQB{


class AccountBalanceStorage{
public:

    /// @brief Structure representing data about an account balance
    struct BalanceData{
        byteBuffer publicKey;
        cash balance;
        uint32_t txSequence;
    };

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
     * @param bd [out] output BalanceData structure
     * @return true if wallet was found
     * @return false if wallet was not found or if database Get operation fails
     */
    bool getBalance(byte64_t &walletID, BalanceData &bd) const;

    /**
     * @brief Put a BalanceData structure to the database.
     * If wallet ID is in the database it will replace the current value,
     * if wallet ID is not in the database it will put new value into the database under walletID key
     * 
     * @param walletID ID of account to which assign the data
     * @param bd balance data structure to put into database
     * @return true if operation was successful
     * @return false if operation fails
     */
    bool setBalance(byte64_t &walletID, BalanceData &bd);

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

    /// @brief Get size of a BalanceData elements (fields) in bytes
    static size_t getSizeOfBalanceData(){
        return sizeof(BalanceData::balance) + sizeof(BalanceData::txSequence) + Signer::GetInstance()->getPublicKeySize();
    }

    /**
     * @brief Serialize BalanceData structure so it can be put into database
     * 
     * @param buffer buffer for serialization
     * @param offset offset to the buffer
     * @param bd Balance data to serialize
     * 
     * @exception If buffer has not enough space for serialized data. This method does not resize the buffer. The buffer has to have enough space, USE getSizeOfBalanceData().
     */
    static void serializeBalanceData(byteBuffer &buffer, size_t &offset, BalanceData &bd);

    /**
     * @brief Deserialize BalanceData queried from the database
     * 
     * @param buffer buffer for deserialization
     * @param offset offset to the buffer
     * @return BalanceData deserialized balance data
     * 
     * @exception If buffer has smaller size then size of BalanceData structure
     */
    static BalanceData deserializeBalanceData(byteBuffer &buffer, size_t &offset);

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

    /// @brief Maximum number of network addresses that can be stored for one account
    static const size_t MAX_ACCOUNT_ADDRESSES = 10;

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
     * @param addresses [out] output vector with addresses
     * @return true if wallet was found
     * @return false if wallet was not found
     */
    bool getAddresses(byte64_t &walletID, std::vector<std::string> &addresses) const;

    /**
     * @brief Put an account addresses to the database.
     * If wallet ID is in the database it will replace the current value,
     * if wallet ID is not in the database it will put new value into the database under walletID key
     * 
     * @param walletID ID of account to which assign the data
     * @param addresses vector of addresses to put into the database
     * @return true if operation was successful
     * @return false if operation fails
     */
    bool setAddresses(byte64_t &walletID, std::vector<std::string> &addresses);

    /// @brief Get size of addresses for serialization (size of addresses when serialized) in bytes
    static size_t getAddressesSize(std::vector<std::string> &addresses);

    /**
     * @brief Serialize vector of an account addresses so it can be put into database. Maximum number of serialized addresses is 10.
     * If given vector of addresses include more than 10 addresses, remaining addresses are ignored.
     * Serialization is in format [uint32_t number of addresses] <[address]'\n',...>. The first field is number of serialized addresses, after that
     * starts the bytes of the first address. Addresses are separated with '\0' byte.
     * 
     * @param buffer buffer for serialization
     * @param offset offset to the buffer
     * @param addresses account addresses to serialize
     * 
     * @exception If buffer has not enough space for serialized data. This method does not resize the buffer. The buffer has to have enough space, USE getAddressesSize().
     */
    static void serializeAddresses(byteBuffer &buffer, size_t &offset, std::vector<std::string> &addresses);

    /**
     * @brief Deserialize vector of an account addresses queried from the database
     * 
     * @param buffer buffer for deserialization
     * @param offset offset to the buffer
     * @return vector with deserialized account addresses. In case of failed deserialization vector of addresses will be empty.
     * 
     * @exception If buffer can not be deserialized because there is not enough space for field with number of addresses, which length is sizeof(uint32_t)
     */
    static std::vector<std::string> deserializeAddresses(byteBuffer &buffer, size_t &offset);

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

    /// @brief Structure representing account information
    struct AccountData{
        AccountBalanceStorage::BalanceData bd;
        std::vector<std::string> addresses;
    };
    
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
     * @param ad [out] Queried account data
     * @return true if operation was successful
     * @return false if operation fails or wallet was not found
     */
    bool getAccount(byte64_t &walletID, AccountData &ad);

    /**
     * @brief Set the Account object
     * 
     * @param walletID ID of an account
     * @param ad AccoundData structure to put into database
     * @return true if operation was successful
     * @return false if operation fails
     */
    bool setAccount(byte64_t &walletID, AccountData &ad);

    /**
     * @brief Serialize AccountData ad into the given buffer. Data are serialized as <[balance data][addresses]>
     * 
     * @param buffer buffer for serialization
     * @param ad AccountData structure to serialize
     */
    void serializeAccount(byteBuffer &buffer, AccountData &ad);

    /**
     * @brief Deserialize account data from byte buffer
     * 
     * @param buffer buffer with serialized data
     * @return AccountData deserialized account data
     * 
     * @exception if serialized data was corrupted
     */
    AccountData deserializeAccount(byteBuffer &buffer);


    AccountBalanceStorage *blncDB; ///< balance database
    AccountAddressStorage *addrDB; ///< address database
};


} // namespace PQB


/* END OF FILE */