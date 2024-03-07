/**
 * @file Connection.hpp
 * @author Michal Ľaš
 * @brief Representation of network connection and manegemnt of connections
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <memory>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Sock.hpp"
#include "Message.hpp"
#include "MessageProcessor.hpp"


namespace PQB{


class ConnectionManager;


/**
 * @brief Each Connection has 2 threads first for receiving messages and second for sending messages to a socket
 * For sending messages there is a message queue. Received messages are proccess in MessageProcessor, which is accessible through ConnectionManager.
 * 
 */
class Connection{
public:

    std::string connID;

    /**
     * @brief Construct a new Connection object
     * 
     * @param socket Sock object representing connected socket
     * @param mangerOfThisConnection ConnectionManager that is managing this Connection
     * @param connectionID Unique identifier of the Connection. Could be the peer ID/address or connection port
     * @param isPeerID Tells if parameter connectionID is a peer ID/address or port number of the connection. True if peer ID, false if port
     * @param isOnUNL tells if this Connection is with peer on the UNL
     */
    Connection(std::string &connectionID, Sock *socket, ConnectionManager *mangerOfThisConnection);
    ~Connection();

    /// @brief send message on this connection 
    void sendMessage(MessageShPtr msg);

private:

    void messageReader();
    void messageSender();

    /// @brief Stop reader, sender threads and notify Connection Manager that this Connection was closed
    void stopConnection();

    /// @brief peek for header data in socket
    /// @return true on success, false on recv failure
    bool peekForHeader(Message::message_hdr_t *header);

    std::atomic<bool> connected;
    std::jthread readerThread;
    std::jthread senderThread;

    std::mutex sendQueueMutex; ///< mutex to sending queue
    std::condition_variable sendCondition;
    std::queue<MessageShPtr> sendMessageQueue;

    Sock *sock;
    ConnectionManager *manager;
};


class ConnectionManager{
    friend class Connection;
public:

    ConnectionManager(MessageProcessor *msgProcessor, std::string &walletID) 
    : messsagProcessor(msgProcessor), thisPeerID(walletID) {}
    ~ConnectionManager() {}

    /**
     * @brief Initialize new Connection to peer (Create a New Connection object)
     * 
     * @param peerID Peer ID/address
     * @param addresses list of peer IP addresses or hostnames
     * @param isOnUNL specify if this new connection is with peer on the UNL (default is false)
     * @return true if connection was successful
     * @return false if connection failed
     */
    bool createNewConnection(std::string &peerID, std::vector<std::string> &addresses);


    bool acceptNewConnection(std::string &port, Sock *socket);

    /// @brief Delete/Close a Connection. connectionID is ID/address of peer to which the connection is made
    void deleteConnection(std::string &connectionID);

    /// @brief Start connections with nodes (peers) on UNL (Unique Node List)
    void initUNLConnections();

    void sendMessageToPeer(std::string &peerID, MessageShPtr message);

    void broadcastMessageToAllPeers(MessageShPtr message);

    void broadcastMessageToUNLPeers(MessageShPtr message);

    void notifyConnectionAck(std::string &peerID);

    void notifyConnectionVersion(std::string &port, std::string &peerID, bool confirmed);

private:

    class ConnectionWrapper{
    public:
        ConnectionWrapper(Connection *connection) : conn(connection) {}

        /// @brief Wrapper for Connection sendMessage method
        void sendMessage(MessageShPtr msg);

        /// @brief Delete Connection object
        void deleteConnection();

        /// @brief Set/Replace connID (connection ID) with a new connection ID
        void setConnectionID(const std::string &newConnectionID);

    private:
        Connection *conn;
        std::mutex mutex;
    };

    MessageProcessor *messsagProcessor;
    std::string thisPeerID; ///< wallet address/peerID/ of local node

    std::vector<std::string> UNLPeerIDs; ///< vector with peerIDs from UNL
    std::shared_mutex UNLPeerIDsMutex;

    std::unordered_map<std::string, std::shared_ptr<ConnectionWrapper>> connectionPool; ///< map of Connections with peers
    std::shared_mutex connectionPoolMutex;

    std::unordered_map<std::string, std::shared_ptr<ConnectionWrapper>> waitingConnectionPool; ///< map of Connections that are waiting for VERSION or ACK message
    std::mutex waitingConnectionPoolMutex;

    /// @brief Add Connection to connectionPool map
    /// @return True if success, False if the peer already has a connection
    bool addConnectionToConnectionPool(const std::string &peerID, const std::shared_ptr<ConnectionManager::ConnectionWrapper> connection);

    /// @brief Delete Connection from peerConnections map
    void deleteConnectionFromConnectionPool(const std::string &peerID);

    /// @brief Get a Connection from connectionPool, return nullptr if not found
    std::shared_ptr<ConnectionWrapper> getConnectionFromConnectionPool(const std::string &peerID);

    /// @brief Check if connection with given ID (peerID) is in connectionPool. True if yes, False if no
    bool isConnectionInConnectionPool(const std::string &peerID);

    /// @brief Add Connection to waitingConnectionPool (connectionID can be either the peerID or the port number of the connection)
    /// @return True if success, False if a connection with the same ID already exists
    bool addConnectionToWaitingConnectionPool(const std::string &connectionID, const std::shared_ptr<ConnectionManager::ConnectionWrapper> connection);

    /// @brief Delete Connection from waitingConnectionPool map. THIS METHOD WILL ALSO DELETE/CLOSE THE CONNECTION !!!
    void deleteConnectionFromWaitingConnectionPool(const std::string &connectionID);

    /// @brief Remove connection from waitingConnectionPool and add it to connectionPool
    /// @param peerID connection ID in waitingConnectionPool
    /// @param newPeerID connection ID that will be placed to connectionPool
    /// @return True if success, False if the connection with the same ID already exists in connectionPool or if connection with given peerID does not exists in waitingConnectionPool
    /// @note This also changes the Connection ID to newPeerID
    bool moveConnectionToConnectionPool(const std::string &peerID, const std::string &newPeerID);
 
    /// @brief Remove connection from waitingConnectionPool and add it to connectionPool (if connection already exists in connectionPool replace it)
    /// @param peerID connection ID in waitingConnectionPool
    /// @param newPeerID connection ID that will be placed to connectionPool
    /// @return True if success, False if connection with given peerID does not exists in waitingConnectionPool
    /// @note This also changes the Connection ID to newPeerID
    bool forceMoveConnectionToConnectionPool(const std::string &peerID, const std::string &newPeerID);

    /**
     * @brief Create connection with a peer
     * 
     * This function was inspired by IBM : Example: IPv4 or IPv6 client
     * Date of last update: 2023-10-10
     * Date of citation: 2024-02-27
     * Ref.: https://www.ibm.com/docs/en/i/7.5?topic=clients-example-ipv4-ipv6-client
     * 
     * @param addresses list of IP addresses or hostnames to try connect to
     * @return Sock* socked object or nullptr in case of failure
     */
    Sock *connectToPeer(std::vector<std::string> &addresses);
};



} // namespace PQB

/* END OF FILE */