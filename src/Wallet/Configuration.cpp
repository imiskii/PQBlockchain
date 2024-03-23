/**
 * @file Configuration.cpp
 * @author Michal Ľaš
 * @brief Functions for reading and writing to a node configuration file
 * @date 2024-03-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Configuration.hpp"
#include "PQBExceptions.hpp"
#include "Log.hpp"


namespace PQB{

    WalletConf::WalletConf(std::string &confFilePath) : filePath(confFilePath){
        std::ifstream confFile(filePath);
        if (!confFile.good()){
            initEmpty();
            PQB_LOG_WARN("WALLET", "Provided configuration file: {} does not exsits! On given path was created new configuration file", filePath);
        }
    }

    void WalletConf::initEmpty(){
        auto j = R"(
        {
            "secretKey": "",
            "publicKey": "",
            "ID": "",
            "addrs": [],
            "UNL": [],
            "balance": 0,
            "sequence": 1,

            "txRecords": []
        }
        )"_json;

        std::ofstream confFile(filePath);
        if (confFile.is_open()){
            confFile << std::setw(4) << j << std::endl;
            confFile.close();
        } else {
            std::string err = "Provided path for configuration file: " + filePath + " is not valid!";
            throw PQB::Exceptions::Wallet(err);
        }
        PQB_LOG_INFO("WALLET", "New wallet created");
    }

    bool WalletConf::loadConf(RawWalletData_t &rwd){
        std::ifstream confFile(filePath);
        if (!confFile.is_open())
            return false;

        nlohmann::json json;
        confFile >> json;

        rwd.walletID = json["ID"];
        rwd.publicKey = json["publicKey"];
        rwd.secretKey = json["secretKey"];
        rwd.balance = json["balance"];
        rwd.txSequenceNumber = json["sequence"];
        rwd.nodeAddresses = json["addrs"];
        rwd.nodeUNL = json["UNL"];
        for (const auto &rawTx : json["txRecords"]){
            RawTransactionData_t tx;
            tx.txID = rawTx["id"];
            tx.senderID = rawTx["sender_id"];
            tx.receiverID = rawTx["receiver_id"];
            tx.cashAmount = rawTx["amount"];
            tx.sequenceNumber = rawTx["seq"];
            tx.timestamp = rawTx["timestamp"];
            tx.version = rawTx["version"];
            tx.confirmed = rawTx["confirmed"];
            rwd.txRecords.push_back(tx);
        }
        confFile.close();
        PQB_LOG_INFO("WALLET", "Wallet configuration loaded");
        return true;
    }

    bool WalletConf::saveConf(RawWalletData_t &rwd){
        std::ofstream confFile(filePath);
        if (!confFile.is_open())
            return false;

        nlohmann::json json;
        json["ID"] = rwd.walletID;
        json["publicKey"] = rwd.publicKey;
        json["secretKey"] = rwd.secretKey;
        json["balance"] = rwd.balance;
        json["sequence"] = rwd.txSequenceNumber;
        json["addrs"] = rwd.nodeAddresses;
        json["UNL"] = rwd.nodeUNL;
        for (const auto &rawTx : rwd.txRecords){
            nlohmann::json jTx;
            jTx["id"] = rawTx.txID;
            jTx["sender_id"] = rawTx.senderID;
            jTx["receiver_id"] = rawTx.receiverID;
            jTx["amount"] = rawTx.cashAmount;
            jTx["seq"] = rawTx.sequenceNumber;
            jTx["timestamp"] = rawTx.timestamp;
            jTx["version"] = rawTx.version;
            jTx["confirmed"] = rawTx.confirmed;
            json["txRecords"].push_back(jTx);
        }

        confFile << std::setw(4) << json << std::endl;
        confFile.close();

        PQB_LOG_INFO("WALLET", "Wallet configuration saved");
        return true;
    }

} // namespace PQB

/* END OF FILE */