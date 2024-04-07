/**
 * @file Wallet.cpp
 * @author Michal Ľaš
 * @brief Management for the local node configuration and resource tracking
 * @date 2024-03-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Wallet.hpp"


namespace PQB{

    Wallet::Wallet(std::string &confFilePath){
        conf = new WalletConf(confFilePath);
    }

    Wallet::~Wallet(){
        delete conf;
    }

    bool Wallet::loadConfigurationFromFile(){
        WalletConf::RawWalletData_t rwd;
        if (!conf->loadConf(rwd))
            return false;

        secretKey.resize(Signer::GetInstance()->getPrivateKeySize());
        publicKey.resize(Signer::GetInstance()->getPublicKeySize());
        hexStringToBytes(rwd.publicKey, publicKey.data(), publicKey.size());
        hexStringToBytes(rwd.secretKey, secretKey.data(), secretKey.size());
        walletID.setHex(rwd.walletID);
        nodeAddresses = rwd.nodeAddresses;
        nodeUNL = rwd.nodeUNL;
        balance = rwd.balance;
        txSequenceNumber = rwd.txSequenceNumber;
        for (auto &tx : rwd.txRecords){
            TransactionData txData;
            txData.senderWalletAddress.setHex(tx.senderID);
            txData.receiverWalletAddress.setHex(tx.receiverID);
            txData.cashAmount = tx.cashAmount;
            txData.sequenceNumber = tx.sequenceNumber;
            txData.timestamp = tx.timestamp;
            txData.versionNumber = tx.version;
            std::pair<TransactionData, TxState> p = std::make_pair(txData, TxState(tx.confirmed));
            txRecords.emplace(tx.txID, p);
        }

        if (walletID.IsNull())
            genNewKeys();

        return true;
    }

    bool Wallet::saveConfigurationToFile(){
        WalletConf::RawWalletData_t rwd;

        rwd.walletID = walletID.getHex();
        rwd.secretKey = bytesToHexString(secretKey.data(), secretKey.size());
        rwd.publicKey = bytesToHexString(publicKey.data(), publicKey.size());
        rwd.balance = balance;
        rwd.txSequenceNumber = txSequenceNumber;
        rwd.nodeUNL = nodeUNL;
        rwd.nodeAddresses = nodeAddresses;
        for (const auto &tx : txRecords){
            WalletConf::RawTransactionData_t rtd;
            rtd.senderID = tx.second.first.senderWalletAddress.getHex();
            rtd.receiverID = tx.second.first.receiverWalletAddress.getHex();
            rtd.cashAmount = tx.second.first.cashAmount;
            rtd.sequenceNumber = tx.second.first.sequenceNumber;
            rtd.version = tx.second.first.versionNumber;
            rtd.timestamp = tx.second.first.timestamp;
            rtd.txID = tx.first;
            rtd.confirmed = uint8_t(tx.second.second);
            rwd.txRecords.push_back(rtd);
        }

        if (conf->saveConf(rwd))
            return true;
        return false;
    }

    TransactionPtr Wallet::createNewTransaction(std::string &receiver, PQB::cash amount){
        signed long newBalance = balance - amount;
        if (newBalance < 0){
            return nullptr;
        }
        
        txSequenceNumber++;
        balance -= amount;

        const auto time = std::chrono::system_clock::now();
        TransactionPtr tx = std::make_shared<PQB::Transaction>();
        tx->versionNumber = TX_VERSION;
        tx->cashAmount = amount;
        tx->sequenceNumber = txSequenceNumber;
        tx->senderWalletAddress = walletID;
        tx->receiverWalletAddress.setHex(receiver);
        tx->timestamp = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
        tx->setHash();
        tx->sign(secretKey);

        std::pair<TransactionData, TxState> p = std::make_pair(tx->getTransactionData(), TxState::WAITING);
        txRecords.emplace(tx->IDHash.getHex(), p);
        return tx;
    }

    void Wallet::updateTransaction(std::string transactionID, TxState status){
        auto it = txRecords.find(transactionID);
        if (it != txRecords.end()){
            it->second.second = status;
            // if transaction was canceled return used amount to wallet
            if (status == TxState::CANCELED)
                addToBlance(it->second.first.cashAmount);
        }
    }

    void Wallet::receivedTransaction(std::string transactionID, TransactionData txData, TxState status){
        if (status == TxState::CONFIRMED){
            addToBlance(txData.cashAmount);
        }
        std::pair<TransactionData, TxState> value = std::make_pair(std::move(txData), status);
        txRecords.emplace(transactionID, value);
    }

    void Wallet::genNewKeys()
    {
        Signer::GetInstance()->genKeys(secretKey, publicKey);
        if (walletID.IsNull())
            HashMan::SHA512_hash(&walletID, publicKey.data(), publicKey.size());
    }

    void Wallet::outputWalletTxRecords(std::stringstream &outStringStream){
        outStringStream 
            << std::setw(33) << std::left << "Transaction ID"
            << std::setw(33) << std::left << "Receiver wallet ID"
            << std::setw(21) << std::left << "Amount"
            << std::setw(21) << std::left << "Tx seq."
            << std::setw(21) << std::left << "Timestamp"
            << std::setw(11) << std::left << "status"
            << std::endl;

        for (const auto &tx : txRecords){
            using clock = std::chrono::system_clock;
            std::time_t time = clock::to_time_t(clock::from_time_t(tx.second.first.timestamp));
            std::tm* tm = std::localtime(&time);
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);

            outStringStream 
            << std::setw(33) << tx.first.substr(0, 32)
            << std::setw(33) << tx.second.first.receiverWalletAddress.getHex().substr(0, 32)
            << std::setw(21) << tx.second.first.cashAmount
            << std::setw(21) << tx.second.first.sequenceNumber
            << std::setw(21) << buffer
            << std::setw(11) << TxStateToString(tx.second.second)
            << std::endl;
        }
    }

} // namespace PQB

/* END OF FILE */