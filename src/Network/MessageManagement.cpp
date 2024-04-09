/**
 * @file MessageManagement.cpp
 * @author Michal Ľaš
 * @brief Processor and Checker for PQB net Messages
 * @date 2024-03-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "MessageManagement.hpp"
#include "Consensus.hpp"
#include "Log.hpp"

namespace PQB
{

    ConnectionManager::ConnectionManager(MessageProcessor *msgProcessor, AccountAddressStorage *addressStorage, Wallet *wallet, NodeType type)
    : messsagProcessor(msgProcessor), addrStorage(addressStorage), wallet_(wallet), localNodeType(type){
        server = new Server();
        runServer();
        connectionManagerRunFlag = true;
        connectionManagerThread = std::jthread(&ConnectionManager::manageConnections, this);
    }

    ConnectionManager::~ConnectionManager(){
        connectionManagerRunFlag = false;
        if (connectionManagerThread.joinable())
            connectionManagerThread.join();
        // Delete all connections
        for (auto &connection : connectionPool){
            delete connection.second;
        }
        // Delete all messages in message requests
        while (!messageRequestQueue.empty()){
            MessageRequest_t req = messageRequestQueue.front();
            delete req.message;
            messageRequestQueue.pop();
        }
        
        delete server;
    }

    void ConnectionManager::addMessageRequest(MessageRequest_t req){
        std::lock_guard<std::mutex> lock(messageRequestQueueMutex);
        messageRequestQueue.push(req);
    }

    void ConnectionManager::addConnectionRequest(ConnectionRequest_t req){
        PQB_LOG_TRACE("CONNECTION MANAGER", "Connection request {} added", shortStr(req.peerID));
        std::lock_guard<std::mutex> lock(connectionRequestQueueMutex);
        connectionRequestQueue.push(req);
    }

    int ConnectionManager::getConnectionID(std::string &peerID){
        Connection *conn = getConnectionFromConnectionPool(peerID);
        if (conn != nullptr){
            return conn->getConnectionSocketFD();
        }
        return -1;
    }

    void ConnectionManager::notifyConnectionVersion(socket_t connectionID, std::string &peerID, bool status){
        // If connection was considered unwanted close it and delete from connectionPool
        if (!status){
            /// The connection can not be deleted right now, because manageConnections() method is iterating through connection set
            socketsToClose.push_back(connectionID);
            PQB_LOG_INFO("CONNECTION MANAGER", "Connection with {} will be closed because of negative status", shortStr(peerID));
            return;
        }

        Connection *thisConn = getConnectionFromConnectionPool(connectionID);

        // Handle duplicit connections
        if (isConnectionInConnectionPool(peerID)){
            // If same connection already exists, then there is need to keep just one of these connections.
            // To decide deterministically local wallet ID (local user ID) and connected peer ID are alphabetically compared.
            // This problem may occure if when two peers initialize connection with each other at the same time, so
            // they send VERSION message to each other and they are both waiting for ACK message.
            if (wallet_->getWalletID().getHex() > peerID){ // If local wallet ID > peer ID, delete new connection and keep existing connection
                socketsToClose.push_back(connectionID);
                PQB_LOG_INFO("CONNECTION MANAGER", "Duplicti connection with {} peer connection will be closed", shortStr(peerID));
                return;
            } else { // Delete existing connection and keep this connection
                Connection *exConn= getConnectionFromConnectionPool(peerID);
                // if existing connection was UNL connection this has to keep at as well
                thisConn->isUNL = exConn->isUNL;
                // Because there will be inserted new peerID in setOfConnectedPeers, existing peerID has to be renamed first and then
                // added to vector with socketIDs to delete. New name will be just the substring of former peerID
                std::string newId = peerID.substr(64);
                updatePeerIdOfConnectionInConnectionPool(exConn->getConnectionSocketFD(), peerID, newId);
                socketsToClose.push_back(exConn->getConnectionSocketFD());
                PQB_LOG_INFO("CONNECTION MANAGER", "Duplicti connection with {} our connection will be closed", shortStr(peerID));
            }
        }
        // Check for UNL connections
        for (const auto &peer : wallet_->getUNL()){
            if (peer == peerID){
                thisConn->isUNL = true;
            }
        }

        PQB_LOG_TRACE("CONNECTION MANAGER", "Connection {} renamed to {}", shortStr(thisConn->connID), shortStr(peerID));
        updatePeerIdOfConnectionInConnectionPool(connectionID, thisConn->connID, peerID);
        thisConn->isConfirmed = true;
        /// Send ACK
        AckMessage *msg = new AckMessage(AckMessage::getPayloadSize());
        msg->serialize(nullptr);
        MessageRequest_t req = {.type=MessageRequestType::ONE, .connectionID=thisConn->getConnectionSocketFD(), .peerID=peerID, .message=msg};
        addMessageRequest(req);
    }

    void ConnectionManager::putConnectionDataToStringStream(std::stringstream &ss){
        ss
        << std::setw(15) << std::left << "Connection ID"
        << std::setw(11) << std::left << "Socket fd"
        << std::setw(14) << std::left << "Is confirmed"
        << std::setw(8) << std::left << "Is UNL"
        << std::endl;

        for (const auto &it : connectionPool){
            Connection *conn = it.second;
            ss
            << std::setw(15) << shortStr(conn->connID)
            << std::setw(11) << it.first
            << std::setw(14) << (conn->isConfirmed ? "true" : "false")
            << std::setw(8) << (conn->isUNL ? "true" : "false") 
            << std::endl;
        }
    }

    bool ConnectionManager::createNewConnection(std::string &peerID, std::vector<std::string> &addresses, bool isOnUNL){
        // Check if this connection does not already exists
        if (isConnectionInConnectionPool(peerID)){
            PQB_LOG_TRACE("CONNECTION MANAGER", "Connection {} alread exists", shortStr(peerID));
            return false;
        }

        // Create new Connection
        Sock *sock = connectToPeer(addresses);
        if (sock == nullptr){
            PQB_LOG_ERROR("CONNECTION MANAGER", "Establishing connection with peer: {} failed", shortStr(peerID));
            return false;
        }
        PQB_LOG_INFO("CONNECTION MANAGER", "Connection with peer: {} established", shortStr(peerID));
        Connection *connection = new Connection(peerID, sock, isOnUNL);
        if (!addConnectionToConnectionPool(sock->getSocketFD(), connection, peerID)){
            delete connection;
            return false;
        }
        connection->isConfirmed = true;
        // Send VERSION message
        VersionMessage *msg = new VersionMessage(VersionMessage::getPayloadSize());
        VersionMessage::version_msg_t mData = {.version=MSG_VERSION, .nodeType=localNodeType, .peerID=wallet_->getWalletID()};
        msg->serialize(&mData);
        MessageRequest_t req = {.type=MessageRequestType::ONE, .connectionID=sock->getSocketFD(), .peerID=peerID, .message=msg};
        addMessageRequest(req);
        return true;
    }

    bool ConnectionManager::acceptNewConnection(std::string &port, Sock *socket){
        Connection *connection = new Connection(port, socket);
        if (!addConnectionToConnectionPool(socket->getSocketFD(), connection, port)){
            delete connection;
            return false;
        }
        return true;
    }

    void ConnectionManager::deleteConnection(const socket_t connectionID){
        auto connection = getConnectionFromConnectionPool(connectionID);
        if (connection != nullptr){
            deleteConnectionFromConnectionPool(connectionID, connection->connID);
            PQB_LOG_INFO("CONNECTION MANAGER", "Connection with {} was closed", shortStr(connection->connID));
            delete connection;
        }
    }

    bool ConnectionManager::sendMessageToPeer(const socket_t connectionID, std::string &peerID, Message *message){
        auto connection = getConnectionFromConnectionPool(connectionID);
        if (connection != nullptr){
            if (connection->connID == peerID && connection->isConfirmed){
                connection->sendMessage(message);
                delete message;
                return true;
            }
        }
        delete message;
        return false;
    }

    void ConnectionManager::broadcastMessageToAllPeers(const socket_t connectionID, std::string &peerID, Message *message){
        for (const auto &conn : connectionPool){
            if (conn.second->isConfirmed){
                if (conn.first != connectionID && conn.second->connID != peerID){
                    conn.second->sendMessage(message);
                }
            }
        }
        delete message;
    }

    void ConnectionManager::broadcastMessageToUNLPeers(const socket_t connectionID, std::string &peerID, Message *message){
        for (const auto &conn : connectionPool){
            if (conn.second->isUNL && conn.second->isConfirmed){
                if (conn.first != connectionID && conn.second->connID != peerID){
                    conn.second->sendMessage(message);
                }
            }
        }
        delete message;
    }

    bool ConnectionManager::addConnectionToConnectionPool(const socket_t connectionID, Connection* connection, const std::string &peerID){
        if (connectionPool.find(connectionID) == connectionPool.end()){
            setOfConnectedPeers.insert(peerID);
            connectionPool[connectionID] = connection;
            addSocketDescriptor(connectionID);
            return true;
        }
        return false;
    }

    void ConnectionManager::deleteConnectionFromConnectionPool(const socket_t connectionID, const std::string &peerID){
        setOfConnectedPeers.erase(peerID);
        connectionPool.erase(connectionID);
        deleteSocketDescriptor(connectionID);
    }

    Connection* ConnectionManager::getConnectionFromConnectionPool(const socket_t connectionID){
        auto it = connectionPool.find(connectionID);
        if (it == connectionPool.end())
            return nullptr;
        else
            return it->second;
    }

    Connection *ConnectionManager::getConnectionFromConnectionPool(const std::string &peerID){
        if (isConnectionInConnectionPool(peerID)){
            for (auto &conn : connectionPool){
                if (conn.second->connID == peerID)
                    return conn.second;
            }
        }
        return nullptr;
    }

    bool ConnectionManager::isConnectionInConnectionPool(const std::string &peerID){
        auto it = setOfConnectedPeers.find(peerID);
        if (it == setOfConnectedPeers.end())
            return false;
        return true;
    }

    bool ConnectionManager::isConnectionInConnectionPool(const socket_t connectionID){
        auto it = connectionPool.find(connectionID);
        if (it == connectionPool.end())
            return false;
        return true;
    }

    bool ConnectionManager::updatePeerIdOfConnectionInConnectionPool(const socket_t connectionID, const std::string &oldPeerID, std::string &newPeerID){
        auto it = connectionPool.find(connectionID);
        if (it != connectionPool.end()){
            setOfConnectedPeers.erase(oldPeerID);
            setOfConnectedPeers.insert(newPeerID);
            it->second->connID = newPeerID;
            return true;
        }
        return false;
    }

    void ConnectionManager::addSocketDescriptor(const socket_t socket_fd){
        struct pollfd newDescriptor = {.fd=socket_fd, .events=POLLIN | POLLPRI, .revents=0};
        socketDescriptors.push_back(newDescriptor);
    }

    void ConnectionManager::deleteSocketDescriptor(const socket_t socket_fd){
        auto it = std::remove_if(socketDescriptors.begin(), socketDescriptors.end(),
            [socket_fd] (const struct pollfd &s) {
                return s.fd == socket_fd;
            });
        if (it != socketDescriptors.end())
            socketDescriptors.erase(it);
    }

    bool ConnectionManager::runServer(){
        Sock *serverSock = server->OpenServer();
        if (serverSock != nullptr){
            struct pollfd serverFD = {.fd=serverSock->getSocketFD(), .events=POLLIN, .revents=0};
            socketDescriptors.insert(socketDescriptors.begin(), serverFD);
            return true;
        }
        return false;
    }

    void ConnectionManager::stopServer(){
        if (!socketDescriptors.empty()){
            if (socketDescriptors[0].fd == server->getSocketFD())
                socketDescriptors.erase(socketDescriptors.begin());
        }
        server->CloseServer();
    }

    Sock *ConnectionManager::connectToPeer(std::vector<std::string> &addresses){
        // Set hints for getaddrinfo()
        struct addrinfo hints, *result = nullptr;
        std::memset(&hints, 0, sizeof(hints));
        hints.ai_flags = AI_NUMERICSERV; // no resulution - service/port is known
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        // hints.ai_protocol = IPPROTO_TCP;
        for (std::string &address : addresses){
            // Determine if the address is IPv4 or IPv6
            struct in6_addr serverAddr;
            if (inet_pton(AF_INET, address.c_str(), &serverAddr) == 1){
                hints.ai_family = AF_INET;
                hints.ai_flags |= AI_NUMERICHOST;
            } else if (inet_pton(AF_INET6, address.c_str(), &serverAddr) == 1){
                hints.ai_family = AF_INET6;
                hints.ai_flags |= AI_NUMERICHOST;
            }
            // getaddrinfo()
            if (getaddrinfo(address.c_str(), std::to_string(PQB_SERVER_PORT).c_str(), &hints, &result) != 0){
                if (result != nullptr){
                    freeaddrinfo(result);
                    result = nullptr;
                }
                continue;
            }
            if (result == nullptr)
                continue;
            // Get socket descriptor and create Sock object
            int socket_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
            if (socket_fd < 0){
                freeaddrinfo(result);
                result = nullptr;
                continue;
            }
            Sock *sock = new Sock(socket_fd);
            // Connect
            PQB_LOG_TRACE("CONNECTION MANAGER", "Trying to create connection with {}", address);
            if (sock->Connect(result->ai_addr, result->ai_addrlen) < 0){
                freeaddrinfo(result);
                result = nullptr;
                delete sock;
                continue;
            }
            freeaddrinfo(result);
            result = nullptr;
            return sock;
        }
        return nullptr;
    }

    void ConnectionManager::manageConnections(){
        PQB_LOG_INFO("CONNECTION MANAGER", "Connection managing thread started");
        int changed;
        while (connectionManagerRunFlag)
        {
            changed = poll(socketDescriptors.data(), socketDescriptors.size(), 1000);

            if (changed < 0){ // poll failure
                /// @todo handle poll failure
                PQB_LOG_ERROR("CONNECTION MANAGER", "poll() function failed");
            } else if (changed > 0 && !socketDescriptors.empty()){ // read sockets
                int isServer = (socketDescriptors.at(0).fd == server->getSocketFD()) ? 1 : 0;
                if ((socketDescriptors.at(0).revents & POLLIN) && (isServer)){
                    handleServerPoll();
                    socketDescriptors.at(0).revents = 0;
                }
                bool closeFlag = false;
                for (auto it = socketDescriptors.begin() + isServer; it != socketDescriptors.end(); ++it){
                    if ((it->revents & (POLLIN | POLLPRI)) != 0){
                        handleConnectionPoll(it->fd, &closeFlag);
                        if (closeFlag){
                            socketsToClose.push_back(it->fd);
                            closeFlag = false;
                            continue;
                        }
                        it->revents = 0;
                    }
                }
                // Remove closed connections
                for (const auto sock_fd : socketsToClose){
                    deleteConnection(sock_fd);
                }
                socketsToClose.clear();
            }
            processMessageQueueRequests();
            processConnectionQueueRequests();
        }
    }
     
    void ConnectionManager::handleServerPoll(){
        std::string clientPort;
        Sock *clientSock = server->AcceptConnection(&clientPort);
        if (clientSock != nullptr)
            acceptNewConnection(clientPort, clientSock);
        
    }

    void ConnectionManager::handleConnectionPoll(int socket_fd, bool *closeFlag){
        Connection *conn = getConnectionFromConnectionPool(socket_fd);
        if (conn != nullptr){
            Message *msg = conn->receiveMessage(closeFlag);
            if (*closeFlag){
                return;
            } else {
                if (msg != nullptr){
                    messsagProcessor->processMessage(socket_fd, conn->connID, conn->isUNL, msg);
                }
            }
        }
    }

    void ConnectionManager::processMessageQueueRequests(){
        std::lock_guard<std::mutex> lock(messageRequestQueueMutex);
        while (!messageRequestQueue.empty()){
            MessageRequest_t req = messageRequestQueue.front();
            messageRequestQueue.pop();

            switch (req.type)
            {
            case MessageRequestType::ONE:
                sendMessageToPeer(req.connectionID, req.peerID, req.message);
                break;
            case MessageRequestType::UNLCAST:
                broadcastMessageToUNLPeers(req.connectionID, req.peerID, req.message);
                break;
            case MessageRequestType::BROADCAST:
                broadcastMessageToAllPeers(req.connectionID, req.peerID, req.message);
                break;           
            default:
                break;
            }
        }
    }

    void ConnectionManager::processConnectionQueueRequests(){
        std::lock_guard<std::mutex> lock(connectionRequestQueueMutex);
        while (!connectionRequestQueue.empty()){
            ConnectionRequest_t req = connectionRequestQueue.front();
            connectionRequestQueue.pop();
            byte64_t peerHash;
            peerHash.setHex(req.peerID);
            AccountAddress accAddrs;
            if (!addrStorage->getAddresses(peerHash, accAddrs)){
                PQB_LOG_ERROR("CONNECTION MANAGER", "{} account is not in the database", shortStr(req.peerID));
                return;
            }
            
            createNewConnection(req.peerID, accAddrs.addresses, req.setAsUNL);
        }
    }

/************************************************************************/
/*************************** MessageProcessor ***************************/
/************************************************************************/


    MessageProcessor::MessageProcessor(AccountStorage *accountStorage, BlocksStorage *blockStorage, ConsensusWrapper *consensusAlgorithm, Wallet *localWallet)
    : connMng(nullptr), accStor(accountStorage), blockStor(blockStorage), consensus(consensusAlgorithm), wallet(localWallet){
        processingRun = true;
        processingThread = std::jthread(&MessageProcessor::messageProcessor, this);
    }

    MessageProcessor::~MessageProcessor(){
        processingRun = false;
        processCondition.notify_one();
        if (processingThread.joinable())
            processingThread.join();
        // Delete all messages from message processing queue
        while (!processingQueue.empty()){
            message_item_t item = processingQueue.top();
            delete item.msg;
            processingQueue.pop();
        }
    }

    void MessageProcessor::processMessage(int connectionID, std::string &peerID, bool isUNL, Message *message){
        MessageType msgType = message->getType(); // save message type for log
        PQB_LOG_TRACE("MESSAGE PROCESSOR", "Message of type {} from peer {} was received for processing", 
                    Message::messageTypeToString(msgType), shortStr(peerID));
        if (connMng != nullptr){
            message_item_t msgi = {.connection_id=connectionID, .peer_id=peerID, .isUNL=isUNL, .msg=message};
            if (!earlyProcessing(msgi)){
                addMessageToProcessingQueue(msgi);
            } else {
                PQB_LOG_TRACE("MESSAGE PROCESSOR", "Message of type {} from peer {} was processed", 
                            Message::messageTypeToString(msgType), shortStr(peerID));
            }
        }
    }

    bool MessageProcessor::checkTransaction(const TransactionPtr tx){
        // Check structure of transaction
        if (!tx->checkTransactionStructure()){
            return false;
        }
        // Check if sender and receiver are not the same
        if (tx->senderWalletAddress == tx->receiverWalletAddress){
            return false;
        }
        AccountBalance accBalance;
        // Check if sender exists
        if (!accStor->blncDB->getBalance(tx->senderWalletAddress, accBalance)){
            return false;
        }
        // Check transaction signature
        if (!tx->verify(accBalance.publicKey)){                    
            return false;        
        }
        // Check if receiver exists
        if (!accStor->blncDB->getBalance(tx->receiverWalletAddress, accBalance)){
            return false;
        }
        return true;
    }

    bool MessageProcessor::checkProposal(const BlockProposalPtr &prop){
        // Check structure of proposalň
        if (!prop->check()){
            return false;
        }
        // Check if issuer exists
        AccountBalance accBalance;
        if (!accStor->blncDB->getBalance(prop->issuer, accBalance)){
            return false;
        }
        // Check proposal signature
        if (!prop->verify(accBalance.publicKey)){
            return false;
        }
        return true;
    }

    bool MessageProcessor::checkProposal(const TxSetProposalPtr &prop){
        // Check structure of proposal
        if (!prop->check()){
            return false;
        }
        // Check if issuer exists
        AccountBalance accBalance;
        if (!accStor->blncDB->getBalance(prop->issuer, accBalance)){
            return false;
        }
        // Check proposal signature
        if (!prop->verify(accBalance.publicKey)){
            return false;
        }
        return true;
    }


    void MessageProcessor::addMessageToProcessingQueue(message_item_t &msgi){
        std::lock_guard<std::mutex> lock(processingQueueMutex);
        processingQueue.push(msgi);
        processCondition.notify_one();
    }

    void MessageProcessor::messageProcessor(){
        PQB_LOG_INFO("MESSAGE PROCESSOR", "Message processing thread started");
        while (processingRun)
        {
            std::unique_lock<std::mutex> lock(processingQueueMutex);
            processCondition.wait(lock, [this]{ return (!processingQueue.empty() || !processingRun); });
            if (!processingRun){
                lock.unlock();
                continue;
            }
            message_item_t msg = processingQueue.top();
            processingQueue.pop();
            lock.unlock();

            MessageType msgType = msg.msg->getType(); // save message type for log
            std::string_view msgSender = shortStr(msg.peer_id);
            lateProcessing(msg);
            PQB_LOG_TRACE("MESSAGE PROCESSOR", "Message of type {} from peer {} was processed", 
                        Message::messageTypeToString(msgType), msgSender);
        }
    }

    bool MessageProcessor::earlyProcessing(message_item_t &msgi){
        switch (msgi.msg->getType())
        {
        case MessageType::VERSION:
            procVersionMessage(msgi);
            return true;
        default:
            return false;
        }
    }

    void MessageProcessor::lateProcessing(message_item_t &msgi){
        switch (msgi.msg->getType())
        {
        case MessageType::TX:
            procTransactionMessage(msgi);
            break;
        case MessageType::BLOCKPROPOSAL:
            procBlockProposalMessage(msgi);
            break;
        case MessageType::TXSETPROPOSAL:
            procTxSetProposalMessage(msgi);
            break;
        case MessageType::BLOCK:
            procBlockMessage(msgi);
            break;
        case MessageType::ACCOUNT:
            procAccountMessage(msgi);
            break;
        case MessageType::INV:
            procInvMessage(msgi);
            break;
        case MessageType::GETDATA:
            procGetDataMessage(msgi);
            break;
        default:
            break;
        }
    }

    void MessageProcessor::procVersionMessage(const message_item_t &msgi){
        VersionMessage *msg = dynamic_cast<VersionMessage*>(msgi.msg);
        if (msg != nullptr){
            VersionMessage::version_msg_t msgData;
            msg->deserialize(&msgData);
            std::string peerID = msgData.peerID.getHex();
            // True is here hardcoded for now, so every connection will be accepted.
            // In case of any changes here can be put any check that can decide if connection should be accepted or not.
            connMng->notifyConnectionVersion(msgi.connection_id, peerID, true);
            delete msgi.msg;
        }
    }

    void MessageProcessor::procTransactionMessage(const message_item_t &msgi){
        TransactionMessage *msg = dynamic_cast<TransactionMessage*>(msgi.msg);
        if (msg != nullptr){
            TransactionPtr msgData = std::make_shared<Transaction>();
            msg->deserialize(msgData.get());
            if (checkTransaction(msgData)){
                if (consensus->addTransactionToPool(msgData) && !waitingData.erase(msgData->IDHash)){ // If not yet processed
                    forwardInvMessage(msgData->IDHash, InvType::TX, msgi);
                }
            }
            delete msgi.msg;
        }
    }

    void MessageProcessor::procBlockProposalMessage(const message_item_t &msgi){
        BlockProposalMessage *msg = dynamic_cast<BlockProposalMessage*>(msgi.msg);
        if (msg != nullptr){
            BlockProposalPtr msgData = std::make_shared<BlockProposal>();
            msg->deserialize(msgData.get());
            if (checkProposal(msgData)){
                consensus->notifyBlockProposal(msgData);
            }
            delete msgi.msg;
        }
    }

    void MessageProcessor::procTxSetProposalMessage(const message_item_t &msgi){
        TxSetProposalMessage *msg = dynamic_cast<TxSetProposalMessage*>(msgi.msg);
        if (msg != nullptr){
            TxSetProposalPtr msgData = std::make_shared<TxSetProposal>();
            msg->deserialize(msgData.get());
            if (checkProposal(msgData)){
                // Create new transaction set and put there exising transaction that were proposed
                // This will save some memory, because there won't be allocated memory for same transactions twice
                TransactionSet newSet;
                for (auto tx : msgData->txSet.txSet){
                    TransactionPtr exTx = consensus->getTransactionFromPool(tx->IDHash);
                    if (exTx != nullptr){
                        newSet.insert(exTx);
                    } else { // Proposed Tx Set include transaction that is not in our transaction pool
                        // Check this transaction
                        if (checkTransaction(tx)){
                            // Insert it to newSet and transaction pool, if it is on waiting list remove it
                            newSet.insert(tx);
                            consensus->addTransactionToPool(tx);
                            auto wit = waitingData.find(tx->IDHash);
                            if (wit != waitingData.end()){
                                waitingData.erase(wit);
                            }
                        }
                    }
                }
                msgData->txSet.txSet.clear();
                msgData->txSet.txSet = newSet;
                consensus->notifyTxSetProposal(msgData);
            }
            delete msgi.msg;
        }
    }

    void MessageProcessor::procBlockMessage(const message_item_t &msgi){
        BlockMessage *msg = dynamic_cast<BlockMessage*>(msgi.msg);
        if (msg != nullptr){
            Block block;
            msg->deserialize(&block);
            byte64_t blockHash = block.getBlockHash();

            const auto it = blockInvQuorum.find(blockHash);
            if (it != blockInvQuorum.end()){
                if (it->second == 0){ // zero means that block was requested
                    /// @todo validate transactions and apply transactions on accounts and add block to storage
                    blockInvQuorum.erase(it);
                }
            }
            delete msgi.msg;
        }
    }

    void MessageProcessor::procAccountMessage(const message_item_t &msgi){
        AccountMessage *msg = dynamic_cast<AccountMessage*>(msgi.msg);
        if (msg != nullptr){
            Account acc;
            msg->deserialize(&acc);
            byte64_t accID = acc.getAccountID();
            const auto wit = waitingData.find(accID);
            if (wit != waitingData.end()){
                waitingData.erase(wit);
                accStor->setAccount(accID, acc);
                forwardInvMessage(accID, InvType::ACCOUNT, msgi);
            } else {
                if (!accStor->getAccount(accID, acc)){
                    accStor->setAccount(accID, acc);
                    forwardInvMessage(accID, InvType::ACCOUNT, msgi);
                }
            }
            delete msgi.msg;
        }
    }

    void MessageProcessor::procInvMessage(const message_item_t &msgi){
        InvMessage *msg = dynamic_cast<InvMessage*>(msgi.msg);
        if (msg != nullptr){
            std::vector<inv_message_t> inventories;
            std::vector<inv_message_t> getDataInventories;
            msg->deserialize(&inventories);
            for (const auto &inv : inventories){
                bool ret = false;
                switch (inv.requestType)
                {
                case InvType::TX:
                    ret = procInvTransaction(inv);
                    break;
                case InvType::BLOCK:
                    ret = procInvBlock(inv);
                    break;
                case InvType::ACCOUNT:
                    ret = procInvAccount(inv);
                    break;
                default:
                    break;
                }
                if (ret){
                    getDataInventories.push_back(inv);
                    // forvard message if we do not have it and it is not a block inv., this ensure that messages will be forwarded only once
                    if (inv.requestType != InvType::BLOCK){
                        forwardInvMessage(inv, msgi);
                    }
                }
            }
            if (!getDataInventories.empty()){
                GetDataMessage *getDataMsg = new GetDataMessage(GetDataMessage::getPayloadSize(getDataInventories.size()));
                getDataMsg->serialize(&getDataInventories);
                ConnectionManager::MessageRequest_t req = {.type=ConnectionManager::MessageRequestType::ONE, .connectionID=msgi.connection_id, .peerID=msgi.peer_id, .message=getDataMsg};
                connMng->addMessageRequest(req);
            }
            delete msgi.msg;
        }
    }

    void MessageProcessor::procGetDataMessage(const message_item_t &msgi){
        // iterate trough items in GetData message and send them to the sender of GetData message
        GetDataMessage *msg = dynamic_cast<GetDataMessage*>(msgi.msg);
        if (msg != nullptr){
            std::vector<inv_message_t> inventories;
            msg->deserialize(&inventories);
            for (const auto &inv : inventories){
                switch (inv.requestType)
                {
                case InvType::TX:
                    procGetTransaction(inv.itemID, msgi);
                    break;
                case InvType::BLOCK:
                    procGetBlock(inv.itemID, msgi);
                    break;
                case InvType::ACCOUNT:
                    procGetAccount(inv.itemID, msgi);
                    break;
                default:
                    break;
                }
            }
            delete msgi.msg;
        }
    }


    bool MessageProcessor::procInvTransaction(const inv_message_t &inv){
        if (waitingData.find(inv.itemID) == waitingData.end()){
            if (!consensus->isTransactionInPool(inv.itemID)){
                waitingData.insert(inv.itemID);
                return true;
            }
        }
        return false;
    }

    bool MessageProcessor::procInvBlock(const inv_message_t &inv){
        auto wit = blockInvQuorum.find(inv.itemID);
        if (wit != blockInvQuorum.end()){
            if (wit->second != 0){
                wit->second++;
                // if block inventory is propageted by < 80% of UNL nodes, then ask the node for this block
                if (wit->second >= (wallet->getUNL().size() * 0.8)){
                    wit->second = 0; // set to zero which signalize that request for this block was made
                    return true;
                }
            }
        } else {
            if (!blockStor->getBlock(inv.itemID)){ // start quorum counting
                blockInvQuorum.emplace(inv.itemID, 1);
            }
        }
        return false;
    }

    bool MessageProcessor::procInvAccount(const inv_message_t &inv){
        if (waitingData.find(inv.itemID) == waitingData.end()){
            Account acc;
            if (!accStor->getAccount(inv.itemID, acc)){
                waitingData.insert(inv.itemID);
                return true;
            }
        }
        return false;
    }

    void MessageProcessor::forwardInvMessage(const byte64_t &item_id, InvType type, const message_item_t &msgi){
        // Create Inventory message for this transaction
        inv_message_t inv = {.requestType=type, .itemID=item_id};
        std::vector<inv_message_t> invVec = {inv};
        InvMessage *invMsg = new InvMessage(InvMessage::getPayloadSize(invVec.size()));
        invMsg->serialize(&invVec);
        // Add request to broadcast this transaction
        ConnectionManager::MessageRequest_t req = {.type=ConnectionManager::MessageRequestType::BROADCAST, .connectionID=msgi.connection_id, .peerID=msgi.peer_id, .message=invMsg};
        connMng->addMessageRequest(req);
    }

    void MessageProcessor::forwardInvMessage(const inv_message_t &inv, const message_item_t &msgi){
        std::vector<inv_message_t> invVec = {inv};
        InvMessage *invMsg = new InvMessage(InvMessage::getPayloadSize(invVec.size()));
        invMsg->serialize(&invVec);
        // Add request to broadcast this transaction
        ConnectionManager::MessageRequest_t req = {.type=ConnectionManager::MessageRequestType::BROADCAST, .connectionID=msgi.connection_id, .peerID=msgi.peer_id, .message=invMsg};
        connMng->addMessageRequest(req);
    }

    void MessageProcessor::procGetTransaction(const byte64_t &tx_id, const message_item_t &msgi){
        TransactionPtr tx = consensus->getTransactionFromPool(tx_id);
        if (tx != nullptr){
            TransactionMessage *msg = new TransactionMessage(tx->getSize());
            msg->serialize(tx.get());
            ConnectionManager::MessageRequest_t req = {.type=ConnectionManager::MessageRequestType::ONE, .connectionID=msgi.connection_id, .peerID=msgi.peer_id, .message=msg};
            connMng->addMessageRequest(req);
        }
    }

    void MessageProcessor::procGetBlock(const byte64_t &block_id, const message_item_t &msgi){
        byteBuffer rawBlock;
        if (blockStor->getRawBlock(block_id, rawBlock)){
            BlockMessage *msg = new BlockMessage(rawBlock.size());
            msg->setRawData(rawBlock);
            ConnectionManager::MessageRequest_t req = {.type=ConnectionManager::MessageRequestType::ONE, .connectionID=msgi.connection_id, .peerID=msgi.peer_id, .message=msg};
            connMng->addMessageRequest(req);
        }
    }

    void MessageProcessor::procGetAccount(const byte64_t &acc_id, const message_item_t &msgi){
        Account acc;
        if (accStor->getAccount(acc_id, acc)){
            AccountMessage *msg = new AccountMessage(acc.getSize());
            msg->serialize(&acc);
            ConnectionManager::MessageRequest_t req = {.type=ConnectionManager::MessageRequestType::ONE, .connectionID=msgi.connection_id, .peerID=msgi.peer_id, .message=msg};
            connMng->addMessageRequest(req);
        }
    }

} // namespace PQB

/* END OF FILE */
