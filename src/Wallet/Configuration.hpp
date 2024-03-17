/**
 * @file Configuration.hpp
 * @author Michal Ľaš
 * @brief Functions for reading and writing to a node configuration file
 * @date 2024-03-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include "PQBtypedefs.hpp"
#include "Transaction.hpp"

namespace PQB{


class WalletConf{
private:

    std::string filePath; ///< Configuration file path


public:

    /// @brief Transaction but with Raw data. senderID and receiverID are hexadecimal strings
    struct RawTransactionData_t{
        uint32_t version;
        PQB::cash cashAmount;
        PQB::timestamp timestamp;
        uint32_t sequenceNumber;
        bool confirmed;
        std::string txID;
        std::string senderID;
        std::string receiverID;
    };

    /// @brief Row Wallet data. PK, SK, walletID are represented as hexadecimal string
    struct RawWalletData_t{
        std::string publicKey;
        std::string secretKey;
        std::string walletID;
        std::vector<std::string> nodeAddresses;
        std::vector<std::string> nodeUNL;

        std::vector<RawTransactionData_t> txRecords;
        cash balance;
        uint32_t txSequenceNumber;
    };


    WalletConf(std::string &confFilePath);

    /// @brief Create empty Wallet configuration file called conf.json
    void initEmpty();
    
    /// @brief Read raw from configuration file
    /// @param rwd [out] structure that will be filled with read data
    /// @return true if operation was successful, false if failed
    bool loadConf(RawWalletData_t &rwd);

    /// @brief Save raw data to configuration file
    /// @param rwd structure of data to write to the file
    /// @return true if operation was successful, false if failed
    bool saveConf(RawWalletData_t &rwd);
};


} // namespace PQB

/* END OF FILE */