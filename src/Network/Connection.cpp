/**
 * @file Connection.cpp
 * @author Michal Ľaš
 * @brief Representation of network connection and manegemnt of connections
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Connection.hpp"
#include "PQBconstatnts.hpp"

namespace PQB{

    class ConnectionManager;

    Connection::Connection(std::string &connectionID, Sock *socket, ConnectionManager *mangerOfThisConnection) 
    : connID(connectionID), sock(socket), manager(mangerOfThisConnection){
        connected = true;
        readerThread = std::jthread(&Connection::messageReader, this);
        senderThread = std::jthread(&Connection::messageSender, this);
    }

    Connection::~Connection(){
        // Threads are stopped at the object destruction
        sock->Shutdown(SHUT_RDWR);
        delete sock;
    }

    void Connection::sendMessage(MessageShPtr msg){
        std::lock_guard<std::mutex> lock(sendQueueMutex);
        sendMessageQueue.push(msg);
        sendCondition.notify_one();
    }

    void Connection::messageReader(){
        Message::message_hdr_t header;
        while (connected)
        {
            if (!peekForHeader(&header))
                continue;            
            
            MessageShPtr newMsg = std::make_shared<Message>(header);
            size_t bytesReceived = 0;
            size_t bufferSize = (newMsg->getSize() < MAX_MESSAGE_SIZE ? newMsg->getSize() : MAX_MESSAGE_SIZE);
            char *buffer = new char[bufferSize];
            std::memset(buffer, 0, bufferSize);

            /// @todo There is not timeout on receiving one large message. If second part of the message is larger than MAX_MESSAGE_SIZE and it does not come the thread will be stuck here
            while (bytesReceived < newMsg->getSize()){
                ssize_t nBytes = sock->Recv(buffer, bufferSize, 0);
                if (nBytes <= 0)
                    break;
                
                newMsg->addFragment(buffer, nBytes);
                bytesReceived += nBytes;
                std::memset(buffer, 0, bufferSize);
            }
            if (bytesReceived == newMsg->getSize() && newMsg->checkMessage())
                manager->messsagProcessor->processMessage(connID, newMsg); // send message to processing
            delete[] buffer;
        }
    }

    void Connection::messageSender()
    {
        while (connected)
        {
            std::unique_lock<std::mutex> lock(sendQueueMutex);
            sendCondition.wait(lock, [this]{ return (!sendMessageQueue.empty() || !connected); });
            if (!connected){
                lock.unlock();
                continue;
            }
            MessageShPtr msg = sendMessageQueue.front();
            sendMessageQueue.pop();
            lock.unlock();

            msg->setCheckSum();
            ssize_t bytesToSend = msg->getSize();
            size_t bytesSended = 0;
            while (bytesToSend > 0){
                ssize_t nBytes = sock->Send(msg->getData() + bytesSended, (bytesToSend < MAX_MESSAGE_SIZE ? bytesToSend : MAX_MESSAGE_SIZE), 0);
                if (nBytes <= 0){
                    break;
                }
                bytesToSend -= nBytes;
                bytesSended += nBytes;
            }
        }
    }

    void Connection::stopConnection(){
        connected = false;
        sendCondition.notify_one();
        readerThread.join();
        senderThread.join();
        manager->deleteConnection(connID);
    }

    bool Connection::peekForHeader(Message::message_hdr_t *header){
        ssize_t nBytes = sock->Recv(header, sizeof(struct Message::message_hdr_t), MSG_PEEK | MSG_WAITALL);
        if (nBytes == 0){
            stopConnection();
            return false; 
        } else if (nBytes < 0)
            return false;
        return header->magicNum == MESSAGE_MAGIC_CONST;
    }

/************************************************************************/
/*************************** ConnectionManager **************************/
/************************************************************************/

    void ConnectionManager::ConnectionWrapper::sendMessage(MessageShPtr msg){
        std::lock_guard<std::mutex> lock(mutex);
        if (conn != nullptr)
            conn->sendMessage(msg);
    }

    void ConnectionManager::ConnectionWrapper::deleteConnection(){
        std::lock_guard<std::mutex> lock(mutex);
        if (conn != nullptr){
            delete conn;
            conn = nullptr;
        }
    }

    void ConnectionManager::ConnectionWrapper::setConnectionID(const std::string &newConnectionID){
        std::lock_guard<std::mutex> lock(mutex);
        if (conn != nullptr){
            conn->connID = newConnectionID;
        }
    }

    bool ConnectionManager::createNewConnection(std::string &peerID, std::vector<std::string> &addresses){
        // Check if this connection does not already exists
        if (isConnectionInConnectionPool(peerID))
            return false;

        // Create new Connection and add it to waitingConnectionsMap
        Sock *sock = connectToPeer(addresses);
        if (sock == nullptr){
            return false;
        }
        Connection *connection = new Connection(peerID, sock, this);
        std::shared_ptr<ConnectionWrapper> connectionWrapper = std::make_shared<ConnectionWrapper>(connection);
        if (!addConnectionToWaitingConnectionPool(peerID, connectionWrapper)){
            connectionWrapper->deleteConnection();
            return false;
        }
        /// @todo send VERSION message
        return true;
    }

    bool ConnectionManager::acceptNewConnection(std::string &port, Sock *socket){
        Connection *connection = new Connection(port, socket, this);
        std::shared_ptr<ConnectionWrapper> connectionWrapper = std::make_shared<ConnectionWrapper>(connection);
        // Add to waiting connections pool, wait for VERSION message
        if (!addConnectionToWaitingConnectionPool(port, connectionWrapper)){
            connectionWrapper->deleteConnection();
            return false;
        }
        return true;
    }

    void ConnectionManager::notifyConnectionAck(std::string &peerID){
        // Check if the same connection isn't in connectionPool (performed in moveConnectionToConnectionPool())
        // If no, move this connection to connectionPool (performed in moveConnectionToConnectionPool())
        if (!moveConnectionToConnectionPool(peerID, peerID)){
            // If yes, alphabetically compare local user ID (address/wallet ID) with the peer address/wallet ID.
            // If local user ID > peer ID, replace exiting connection in connectionPool with this connection
            // else close this connection.
            // This step is done to deterministically decide the connection between two peers
            // The problem is that when two peers initialize connection with each other at the same time, so
            // they send VERSION message to each other and they are both waiting for ACK message, there has to be
            // a mechanism that decide which of these 2 initialized connections will stay and which will be closed.
            if (thisPeerID > peerID){
                forceMoveConnectionToConnectionPool(peerID, peerID);
            } else {
                deleteConnectionFromWaitingConnectionPool(peerID);
            }
        }
    }

    void ConnectionManager::notifyConnectionVersion(std::string &port, std::string &peerID, bool confirmed){
        // If connection was considered unwanted close it and delete from waitingConnectionPool
        if (!confirmed)
            deleteConnectionFromWaitingConnectionPool(port);

        // Check if the same connection isn't in connectionPool (performed in moveConnectionToConnectionPool())
        if (moveConnectionToConnectionPool(port, peerID)){ // if no, send ACK and add it to connectionPool
            /// @todo send ACK to peer (peerID)

        } else { // if yes, close this connection
            deleteConnectionFromWaitingConnectionPool(port);
        }
    }

    void ConnectionManager::deleteConnection(std::string &connectionID){
        auto connection = getConnectionFromConnectionPool(connectionID);
        if (connection){
            deleteConnectionFromConnectionPool(connectionID);
            connection->deleteConnection();
        }
    }

    void ConnectionManager::initUNLConnections(){
        /// @todo need reference to storage/UNL list
        // check if connection is in connesctions
        // if not create new connection
    }

    void ConnectionManager::sendMessageToPeer(std::string &peerID, MessageShPtr message){
        auto connection = getConnectionFromConnectionPool(peerID);
        if (connection){
            connection->sendMessage(message);
        }
    }

    void ConnectionManager::broadcastMessageToAllPeers(MessageShPtr message){
        std::shared_lock<std::shared_mutex> lock(connectionPoolMutex);
        for (const auto &el : connectionPool){
            el.second->sendMessage(message);
        }
    }

    void ConnectionManager::broadcastMessageToUNLPeers(MessageShPtr message){
        std::shared_lock<std::shared_mutex> lock(UNLPeerIDsMutex);
        for (const auto &peerID : UNLPeerIDs){
            auto connection = getConnectionFromConnectionPool(peerID);
            if (connection != nullptr)
                connection->sendMessage(message);
        }
    }

    Sock *ConnectionManager::connectToPeer(std::vector<std::string> &addresses)
    {
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
            if (getaddrinfo(address.c_str(), std::to_string(PQB_SERVER_PORT).c_str(), &hints, &result) != 0)
                continue;
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
            if (sock->Connect(result->ai_addr, result->ai_addrlen) < 0){
                freeaddrinfo(result);
                result = nullptr;
                delete sock;
                continue;
            }
            return sock;
        }
        return nullptr;
    }

    bool ConnectionManager::addConnectionToConnectionPool(const std::string &peerID, const std::shared_ptr<ConnectionWrapper> connection){
        std::lock_guard<std::shared_mutex> lock(connectionPoolMutex);
        auto it = connectionPool.find(peerID);
        if (it == connectionPool.end()){
            connectionPool.insert({peerID, connection});
            return true;
        }
        return false;
    }

    void ConnectionManager::deleteConnectionFromConnectionPool(const std::string & peerID){
        std::lock_guard<std::shared_mutex> lock(connectionPoolMutex);
        connectionPool.erase(peerID);
    }

    std::shared_ptr<ConnectionManager::ConnectionWrapper> ConnectionManager::getConnectionFromConnectionPool(const std::string &peerID){
        std::shared_lock<std::shared_mutex> lock(connectionPoolMutex);
        auto it = connectionPool.find(peerID);
        if (it == connectionPool.end())
            return nullptr;
        else
            return it->second;
    }

    bool ConnectionManager::isConnectionInConnectionPool(const std::string &peerID){
        std::shared_lock<std::shared_mutex> lock(connectionPoolMutex);
        auto it = connectionPool.find(peerID);
        if (it == connectionPool.end())
            return false;
        return true;
    }

    bool ConnectionManager::addConnectionToWaitingConnectionPool(const std::string &connectionID, const std::shared_ptr<ConnectionManager::ConnectionWrapper> connection){
        std::lock_guard<std::mutex> lock(waitingConnectionPoolMutex);
        auto it = waitingConnectionPool.find(connectionID);
        if (it == waitingConnectionPool.end()){
            waitingConnectionPool.insert({connectionID, connection});
            return true;
        }
        return false;
    }

    void ConnectionManager::deleteConnectionFromWaitingConnectionPool(const std::string &connectionID){
        std::lock_guard<std::mutex> lock(waitingConnectionPoolMutex);
        auto it = waitingConnectionPool.find(connectionID);
        if (it != waitingConnectionPool.end()){
            it->second->deleteConnection();
            waitingConnectionPool.erase(connectionID);
        }
    }

    bool ConnectionManager::moveConnectionToConnectionPool(const std::string &peerID, const std::string &newPeerID){
        std::lock_guard<std::mutex> lockWCP(waitingConnectionPoolMutex);
        auto it_wcp = waitingConnectionPool.find(peerID);
        if (it_wcp == waitingConnectionPool.end())
            return false;

        std::lock_guard<std::shared_mutex> lockCP(connectionPoolMutex);
        auto it_cp = connectionPool.find(newPeerID);
        if (it_cp != connectionPool.end()){
            return false;
        }

        it_wcp->second->setConnectionID(newPeerID);
        connectionPool.insert({newPeerID, it_wcp->second});
        waitingConnectionPool.erase(peerID);
        return true;
    }

    bool ConnectionManager::forceMoveConnectionToConnectionPool(const std::string &peerID, const std::string &newPeerID){
        std::lock_guard<std::mutex> lockWCP(waitingConnectionPoolMutex);
        auto it_wcp = waitingConnectionPool.find(peerID);
        if (it_wcp == waitingConnectionPool.end())
            return false;

        std::lock_guard<std::shared_mutex> lockCP(connectionPoolMutex);
        auto it_cp = connectionPool.find(newPeerID);
        if (it_cp != connectionPool.end()){
            it_cp->second->deleteConnection();
            connectionPool.erase(newPeerID);
        }

        it_wcp->second->setConnectionID(newPeerID);
        connectionPool.insert({newPeerID, it_wcp->second});
        waitingConnectionPool.erase(peerID);
        return true;
    }


} // namespace PQB

/* END OF FILE */