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

    Connection::Connection(std::string &connectionID, Sock *socket, bool isOnUNL) 
    : connID(connectionID), sock(socket){
        isConfirmed = false;
        isUNL = isOnUNL;
    }

    Connection::~Connection(){
        sock->Shutdown(SHUT_RDWR);
        delete sock;
    }

    void Connection::sendMessage(Message *message){
        /// @todo calsulation of checksum place to MessageCreator
        message->setCheckSum();
        ssize_t bytesToSend = message->getSize();
        ssize_t bytesSent = 0;
        while (bytesToSend > 0){
            ssize_t nBytes = sock->Send(message->getData() + bytesSent, (bytesToSend < MAX_MESSAGE_SIZE ? bytesToSend : MAX_MESSAGE_SIZE), 0);
            if (nBytes <= 0){
                break;
            }
            bytesToSend -= nBytes;
            bytesSent += nBytes;
        }
    }

    Message *Connection::receiveMessage(bool *closeFlag){
        *closeFlag = false;
        Message::message_hdr_t header;
        if (!peekForHeader(&header, closeFlag))
            return nullptr;

        Message *newMsg = new Message(header);
        size_t bytesReceived = 0;
        size_t bufferSize = (newMsg->getSize() < MAX_MESSAGE_SIZE ? newMsg->getSize() : MAX_MESSAGE_SIZE);
        char *buffer = new char[bufferSize];
        std::memset(buffer, 0, bufferSize);

        /// @todo There is not timeout on receiving one large message. If second part of the message is larger than MAX_MESSAGE_SIZE and it does not come the thread will be stuck here
        /// A problem may occure also if recived message is not complete because other parts do not came yet
        while (bytesReceived < newMsg->getSize()){
            ssize_t nBytes = sock->Recv(buffer, bufferSize, 0);
            if (nBytes <= 0)
                break;
            
            newMsg->addFragment(buffer, nBytes);
            bytesReceived += nBytes;
            std::memset(buffer, 0, bufferSize);
        }
        delete[] buffer;
        if (bytesReceived == newMsg->getSize() && newMsg->checkMessage() && filterMessage(newMsg)){
            return newMsg;
        }
        delete newMsg;
        return nullptr;
    }

    bool Connection::peekForHeader(Message::message_hdr_t *header, bool *closeFlag){
        *closeFlag = false;
        /// @todo adjust the parsing
        ssize_t nBytes = sock->Recv(header, sizeof(struct Message::message_hdr_t), MSG_PEEK);
        if (nBytes == 0){
            *closeFlag = true;
            return false;
        } else if (nBytes < 0)
            return false;
        return header->magicNum == MESSAGE_MAGIC_CONST;
    }

    bool Connection::filterMessage(Message *message){
        switch (message->getType())
        {
        case MessageType::ACK:
            isConfirmed = true;
            return false;
        case MessageType::VERSION:
            return isConfirmed ? false : true;
        default:
            return true;
            break;
        }
    }

/************************************************************************/
/*************************** ConnectionManager **************************/
/************************************************************************/

    ConnectionManager::ConnectionManager(MessageProcessor *msgProcessor, std::string &walletID)
    : messsagProcessor(msgProcessor), localWalletID(walletID){
        server = new Server();
        if (!runServer()){
            /// @todo make log
        }
    }

    ConnectionManager::~ConnectionManager(){
        delete server;
    }

    void ConnectionManager::addMessageRequest(MessageRequest_t req){
        std::lock_guard<std::mutex> lock(messageRequestQueueMutex);
        messageRequestQueue.push(req);
    }

    void ConnectionManager::addConnectionRequest(std::string &peerID){
        std::lock_guard<std::mutex> lock(connectionRequestQueueMutex);
        connectionRequestQueue.push(peerID);
    }

    int ConnectionManager::getConnectionID(std::string &peerID){
        Connection *conn = getConnectionFromConnectionPool(peerID);
        if (conn != nullptr){
            return conn->getConnectionSocketFD();
        }
        return -1;
    }

    void ConnectionManager::initUNLConnections()
    {
        /// @todo need reference to storage/UNL list
        // check if connection is in connesctions
        // if not create new connection
        // fill vector with UNL connections
    }

    void ConnectionManager::notifyConnectionVersion(socket_t connectionID, std::string &peerID, bool status){
        // If connection was considered unwanted close it and delete from connectionPool
        if (!status){
            deleteConnection(connectionID);
            return;
        }

        Connection *thisConn = getConnectionFromConnectionPool(connectionID);

        // Handle duplicit connections
        if (isConnectionInConnectionPool(peerID)){
            // If same connection already exists, then there is need to keep just one of these connections.
            // To decide deterministically local wallet ID (local user ID) and connected peer ID are alphabetically compared.
            // This problem may occure if when two peers initialize connection with each other at the same time, so
            // they send VERSION message to each other and they are both waiting for ACK message.
            if (localWalletID > peerID){ // If local wallet ID > peer ID, delete new connection and keep existing connection
                deleteConnection(connectionID);
                return;
            } else { // Delete existing connection and keep this connection
                Connection *exConn= getConnectionFromConnectionPool(peerID);
                deleteConnection(exConn->getConnectionSocketFD());
            }
        }
        updatePeerIdOfConnectionInConnectionPool(connectionID, thisConn->connID, peerID);
        thisConn->isConfirmed = true;
        /// @todo send ACK
        
    }

    bool ConnectionManager::createNewConnection(std::string &peerID, std::vector<std::string> &addresses){
        // Check if this connection does not already exists
        if (isConnectionInConnectionPool(peerID))
            return false;

        // Create new Connection
        Sock *sock = connectToPeer(addresses);
        if (sock == nullptr){
            return false;
        }
        Connection *connection = new Connection(peerID, sock);
        if (!addConnectionToConnectionPool(sock->getSocketFD(), connection, peerID)){
            delete connection;
            return false;
        }
        connection->isConfirmed = true;
        /// @todo send VERSION message
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
            delete connection;
        }
    }

    bool ConnectionManager::sendMessageToPeer(const socket_t connectionID, std::string &peerID, Message *message){
        auto connection = getConnectionFromConnectionPool(connectionID);
        if (connection != nullptr){
            if (connection->connID == peerID && connection->isConfirmed){
                connection->sendMessage(message);
                return true;
            }
        }
        return false;
    }

    void ConnectionManager::broadcastMessageToAllPeers(Message *message){
        for (const auto &conn : connectionPool){
            if (conn.second->isConfirmed)
                conn.second->sendMessage(message);
        }
    }

    void ConnectionManager::broadcastMessageToUNLPeers(Message *message){
        for (const auto &conn : connectionPool){
            if (conn.second->isUNL && conn.second->isConfirmed)
                conn.second->sendMessage(message);
        }
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

    void ConnectionManager::messageReader(){
        int changed;
        while (messageReaderRunFlag)
        {
            changed = poll(socketDescriptors.data(), socketDescriptors.size(), 0);

            if (changed < 0){ // poll failure
                /// @todo handle poll failure
                /// @todo make log
            } else if (changed > 0 && !socketDescriptors.empty()){ // read sockets
                int isServer = (socketDescriptors.at(0).fd == server->getSocketFD()) ? 1 : 0;
                if ((socketDescriptors.at(0).revents & POLLIN) && (isServer)){
                    handleServerPoll();
                    socketDescriptors.at(0).revents = 0;
                }
                for (auto it = socketDescriptors.begin() + isServer; it != socketDescriptors.end(); ++it){
                    if ((it->revents & (POLLIN | POLLPRI)) != 0){
                        handleConnectionPoll(it->fd);
                        it->revents = 0;
                    }
                }
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

    void ConnectionManager::handleConnectionPoll(int socket_fd){
        Connection *conn = getConnectionFromConnectionPool(socket_fd);
        if (conn != nullptr){
            bool closeFlag;
            Message *msg = conn->receiveMessage(&closeFlag);
            if (closeFlag){
                deleteConnection(socket_fd);
            } else {
                if (msg != nullptr)
                    messsagProcessor->processMessage(socket_fd, conn->connID, msg);
            }
        }
    }

    void ConnectionManager::processMessageQueueRequests(){
        std::lock_guard<std::mutex> lock(messageRequestQueueMutex);
        while (messageRequestQueue.empty()){
            MessageRequest_t req = messageRequestQueue.front();
            messageRequestQueue.pop();

            switch (req.type)
            {
            case MessageRequestType::ONE:
                sendMessageToPeer(req.connectionID, req.peerID, req.message);
                break;
            case MessageRequestType::UNLCAST:
                broadcastMessageToUNLPeers(req.message);
                break;
            case MessageRequestType::BROADCAST:
                broadcastMessageToAllPeers(req.message);
                break;           
            default:
                break;
            }
        }
    }

    void ConnectionManager::processConnectionQueueRequests(){
        std::lock_guard<std::mutex> lock(connectionRequestQueueMutex);
        while (connectionRequestQueue.empty()){
            std::string connID = connectionRequestQueue.front();
            connectionRequestQueue.pop();
            /// @todo query from database
            // createNewConnection(connID, );
        }
    }

} // namespace PQB

/* END OF FILE */