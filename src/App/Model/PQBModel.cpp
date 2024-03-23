/**
 * @file PQBModel.cpp
 * @author Michal Ľaš (xlasmi00@vutbr.cz)
 * @brief Functions/Methods for operations with PQ Blockchain
 * @date 2024-03-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "PQBModel.hpp"
#include "Log.hpp"

namespace PQB{

    PQBModel::PQBModel(std::string &config_file_path){
       wallet = new Wallet(config_file_path);
       blockS = new BlocksStorage();
       accS = new AccountStorage();
       consensus = nullptr;
       connMng = nullptr;
       msgPrc = nullptr;
    }

    PQBModel::~PQBModel(){
        wallet->saveConfigurationToFile();
        delete wallet;
        delete blockS;
        delete accS;
        if (consensus)
            delete consensus;
        if (connMng)
            delete connMng;
        if (msgPrc)
            delete msgPrc;
    }

    bool PQBModel::initializeManagers(NodeType node_type){
        if (!openConfigurationAndDatabase()){
            return false;
        }
        consensus = new Consensus();
        msgPrc = new MessageProcessor(accS, blockS, consensus, wallet);
        std::string walletID = wallet->getWalletID().getHex();
        connMng = new ConnectionManager(msgPrc, accS->addrDB, walletID, node_type);
        msgPrc->assignConnectionManager(connMng);
        return true;
    }

    void PQBModel::initializeUNLConnections(){
        std::vector<std::string> unl = wallet->getUNL();
        for (const auto &peer : unl){
            PQB::ConnectionManager::ConnectionRequest_t req = {.peerID=peer, .setAsUNL=true};
            connMng->addConnectionRequest(req);
        }
    }

    bool PQBModel::openConfigurationAndDatabase(){
        if (!wallet->loadConfigurationFromFile()){
            return false;
        }
        try{
            blockS->openDatabase();
            accS->openDatabases();
        } catch (PQB::Exceptions::Storage &err){
            return false;
        }
        return true;
    }

    std::string PQBModel::createTransaction(std::string &receiver, PQB::cash amount){
        TransactionPtr tx = wallet->createNewTransaction(receiver, amount);
        if (tx == nullptr){
            return "Transaction can not be created because of missing balance!";
        }
        if (!msgPrc->checkTransaction(tx)){
            return "Transaction can not be sent because of invalid transaction structure, sender or receiver address or invalid signature!";
        }
        TransactionMessage *msg = new TransactionMessage(tx->getSize());
        msg->serialize(tx.get());
        ConnectionManager::MessageRequest_t req = {.type=ConnectionManager::MessageRequestType::BROADCAST, .connectionID=0, .peerID="", .message=msg};
        connMng->addMessageRequest(req);
        consensus->addTransactionToPool(tx);
        return "Transaction was successfully created.";
    }

} // namespace PQB

/* END OF FILE */