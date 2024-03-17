/**
 * @file Wallet.hpp
 * @author Michal Ľaš
 * @brief Management for the local node configuration and resource tracking
 * @date 2024-03-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <vector>
#include <string>
#include <map>
#include <unistd.h>
#include <chrono>
#include <utility>
#include <sstream>
#include <memory>
#include "PQBconstatnts.hpp"
#include "PQBtypedefs.hpp"
#include "Transaction.hpp"
#include "HashManager.hpp"
#include "Signer.hpp"
#include "Configuration.hpp"


namespace PQB{


struct TransactionDataTimestampComparator;


/// @brief Data stored in a wallet
class WalletData{

protected:
    byteBuffer publicKey;
    byteBuffer secretKey;
    byte64_t walletID;
    std::vector<std::string> nodeAddresses;
    std::vector<std::string> nodeUNL;
    
    /// @brief Pool of transactions made with this wallet, the bool indicates if transaction is confirmed and included in a block or not
    std::map<std::string, std::pair<TransactionData, bool>> txRecords;
    PQB::cash balance;
    uint32_t txSequenceNumber;

public:

    void setNull(){
        publicKey.clear();
        secretKey.clear();
        walletID.SetNull();
        nodeAddresses.clear();
        nodeUNL.clear();
        balance = 0;
        txSequenceNumber = 1;
    }


    byteBuffer& getPublicKey() { return publicKey; }
    byteBuffer& getSecretKey() { return secretKey; }
    byte64_t& getWalletID() { return walletID; }
    std::vector<std::string>& getAddressList() { return nodeAddresses; }
    std::vector<std::string>& getUNL() { return nodeUNL; }

};


class Wallet : public WalletData{
private:

    WalletConf *conf; ///< Configuration file reader and writer

    WalletConf::RawWalletData_t getRowDataFrom();

public:

    Wallet(std::string &confFilePath);
    ~Wallet();

    /// @brief Read wallet data from a configuration file
    /// @return true if operation was successful, false if failed 
    bool loadConfigurationFromFile();

    /// @brief Save wallet data to the configuration file
    /// @return true if operation was successful, false if failed 
    bool saveConfigurationToFile();

    /// @brief Create a transaction with this wallet
    /// @param receiver Hexadecimal string representing address wallet of the receiver
    /// @param amount Amount of resources to transfer to the receiver
    /// @return New created Transaction 
    TransactionPtr createNewTransaction(std::string &receiver, PQB::cash amount);

    /// @todo confirmTx, cancelTx, printTxRecords, keep track if transaction is confirmed or not

    /// @brief Set transaction in transaction records as confirmed
    /// @param transactionID hexadecimal id of a transaction
    void confirmTransaction(std::string &transactionID);
    
    /// @brief Generate new pair of keys for making digital signatures
    void genNewKeys();

    /// @brief Serialize and return printable string of transactions made with this wallet
    std::string outputWalletTxRecords();
};

/// @brief Comparator to order transactions by time they was made
struct TransactionDataTimestampComparator{
    bool operator()(const TransactionData& a, const TransactionData& b) const{
        return a.timestamp < b.timestamp;
    }
};

} // namespace PQB

/* END OF FILE */