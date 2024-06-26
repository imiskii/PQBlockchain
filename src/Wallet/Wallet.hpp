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
#include "PQBconstants.hpp"
#include "PQBtypedefs.hpp"
#include "Transaction.hpp"
#include "HashManager.hpp"
#include "Signer.hpp"
#include "Configuration.hpp"


namespace PQB{


struct TransactionDataTimestampComparator;


/// @brief Data stored in a wallet
class WalletData{
public:

    enum class TxState : uint8_t{
        CANCELED = 0,
        CONFIRMED = 1,
        WAITING = 2
    };

    static std::string TxStateToString(const TxState &state){
        switch (state)
        {
        case TxState::CANCELED:
            return "canceled";
        case TxState::CONFIRMED:
            return "confirmed";
        case TxState::WAITING:
            return "waiting";
        default:
            return "unknow";
        }
    }

protected:
    byteBuffer publicKey;
    byteBuffer secretKey;
    byte64_t walletID;
    std::vector<std::string> nodeAddresses;
    std::vector<std::string> nodeUNL;
    
    /// @brief Pool of transactions made with this wallet, the bool indicates if transaction is confirmed and included in a block or not
    std::map<std::string, std::pair<TransactionData, TxState>> txRecords;
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

    PQB::cash getBalance() { return balance; }
    u_int32_t getTxSeqNum() { return txSequenceNumber; }
    byteBuffer& getPublicKey() { return publicKey; }
    byteBuffer& getSecretKey() { return secretKey; }
    byte64_t& getWalletID() { return walletID; }
    std::vector<std::string>& getAddressList() { return nodeAddresses; }
    std::vector<std::string>& getUNL() { return nodeUNL; }
    void setBalance(PQB::cash newBalance) { balance = newBalance; }
    void addToBlance(PQB::cash cashToAdd) { balance += cashToAdd; }

};


class Wallet : public WalletData{
private:

    WalletConf *conf; ///< Configuration file reader and writer

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
    /// @return New created Transaction, nullptr if transaction can not be created because of missing balance (cash in wallet)
    TransactionPtr createNewTransaction(std::string &receiver, PQB::cash amount);

    /// @todo confirmTx, cancelTx, printTxRecords, keep track if transaction is confirmed or not

    /// @brief Set transaction in transaction records given `status`
    /// @param transactionID hexadecimal id of a transaction
    /// @param status status to assign
    void updateTransaction(std::string transactionID, TxState status);

    /// @brief Add transaction to transaction records
    /// @param transactionID identifier of the transaction
    /// @param txData transaction data
    /// @param status status of transaction
    void receivedTransaction(std::string transactionID, TransactionData txData, TxState status);
    
    /// @brief Generate new pair of keys for making digital signatures
    void genNewKeys();

    /// @brief Serialize transactions made with this wallet and put them into `outStringStream` stream
    /// @param outStringStream [out] string stream where to put serialized wallet transactions
    void outputWalletTxRecords(std::stringstream &outStringStream);
};

/// @brief Comparator to order transactions by time they was made
struct TransactionDataTimestampComparator{
    bool operator()(const TransactionData& a, const TransactionData& b) const{
        return a.timestamp < b.timestamp;
    }
};

} // namespace PQB

/* END OF FILE */