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
#include <set>
#include <algorithm>
#include <thread>
#include <mutex>
#include <atomic>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include "Sock.hpp"
#include "Server.hpp"
#include "Message.hpp"
#include "MessageProcessor.hpp"


namespace PQB{


class ConnectionManager;


/// @brief Class representing a connection with a peer
class Connection{
public:

    std::string connID;
    bool isConfirmed;
    bool isUNL;

    /**
     * @brief Construct a new Connection object
     * 
     * @param connectionID Unique identifier of the Connection. Could be the peer ID/address or temporarily connection port (while VERSION message is received)
     * @param socket Sock object representing connected socket
     * @param isOnUNL tells if this Connection is with peer on the UNL (default false)
     */
    Connection(std::string &connectionID, Sock *socket, bool isOnUNL = false);
    ~Connection();

    /// @brief Send message to the peer on this connection 
    void sendMessage(Message *message);

    /// @brief Read one message sent from peer
    /// @param[out] closeFlag Set to true if connection has closed (recv() returns 0)
    /// @return Pointer to Message or nullptr on failure
    Message *receiveMessage(bool *closeFlag);

    /// @brief Return socked descriptor of this Connection
    socket_t getConnectionSocketFD(){
        return sock->getSocketFD();
    }

private:

    /// @brief Peek for header data in socket
    /// @param[out] header Parsed message header
    /// @param[out] closeFlag Set to true if connection has closed (recv() returns 0)
    /// @return true on success, false on recv failure
    bool peekForHeader(Message::message_hdr_t *header, bool *closeFlag);

    /// @brief Early filter for messages
    /// @return True if message pass, false if message do not pass
    bool filterMessage(Message *message);

    Sock *sock;
};


/// @brief Class for managing connections with peers
class ConnectionManager{

public:

    /// @brief Possible messages types for MessageProcessor
    enum class MessageRequestType{
        ONE,
        BROADCAST,
        UNLCAST
    };

    /// @brief Datatype representing a message request. Message requests are made for ConnectionManager that will process it.
    struct MessageRequest_t{
        MessageRequestType type; ///< Type of request
        socket_t connectionID;   ///< ID of connection where the message will be sent, ignored if UNLCAST or BROADCASR message type
        std::string peerID;      ///< ID of peer to which the message will be sent, ignored if UNLCAST or BROADCASR message type    
        Message *message;        ///< Message to sent
    };

    ConnectionManager(MessageProcessor *msgProcessor, std::string &walletID);

    ~ConnectionManager();

    /**
     * @brief Add a request for sending a message to the ConnectionMannager
     * 
     * @param req Message request to send
     */
    void addMessageRequest(MessageRequest_t req);

    /**
     * @brief Add request for creating connection with a peer
     * 
     * @param peerID peer ID with which connection should be established
     */
    void addConnectionRequest(std::string &peerID);

    /**
     * @brief Find connection ID of a peer
     * 
     * @param peerID address/ID of a peer
     * @return int connectionID (socket descriptor of the connection) or -1 if not found
     */
    int getConnectionID(std::string &peerID);

    /**
     * @brief Notify this ConnectionManager that VERSION message was processed
     * 
     * @param connectionID ID of connection where VERSION message was received
     * @param peerID ID of the peer from VERSION message
     * @param status The result of the processing. True if connection should be accepted, false if connection should be closed
     */
    void notifyConnectionVersion(socket_t connectionID, std::string &peerID, bool status);

private:

    MessageProcessor *messsagProcessor;
    std::string localWalletID; ///< wallet address/peerID of local (this) node
    Server *server; ///< Instance of a PQB server

    std::queue<std::string> connectionRequestQueue; ///< queue with requests for connection
    std::mutex connectionRequestQueueMutex;
    std::queue<MessageRequest_t> messageRequestQueue; ///< queue with requests for ConnectionManager
    std::mutex messageRequestQueueMutex;

    std::jthread connectionManagerThread;
    std::atomic_bool connectionManagerRunFlag; ///< true if message connection manager should run, false if should stop

    std::unordered_map<socket_t, Connection*> connectionPool; ///< socket descriptors mapped to Connections
    std::vector<struct pollfd> socketDescriptors; ///< vector of sokcet descriptors
    std::set<std::string> setOfConnectedPeers; ///< addresses of connected peers (use for check that just one connection with peer is kept)

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

    /**
     * @brief Add connection to connections pool
     * 
     * @param port Port of the connection. Serves as temporory Connection ID
     * @param socket Sock object of the new connection
     * @return true if new connection was created successfuly
     * @return false if failed creating new connection
     */
    bool acceptNewConnection(std::string &port, Sock *socket);
    
    /// @brief Start connections with nodes (peers) on UNL (Unique Node List)
    void initUNLConnections();

    /**
     * @brief Delete/Close a Connection
     * 
     * @param connectionID ID/address of peer to which the connection is made
     */
    void deleteConnection(const socket_t connectionID);

    /**
     * @brief Send a message to a peer
     * 
     * @param connectionID ID of the connection (socked descriptor of the connection)
     * @param peerID ID of peer to which the message is addressed
     * @param message message to be sent
     * @return true If sending was successful
     * @return false if sending failed
     */
    bool sendMessageToPeer(const socket_t connectionID, std::string &peerID, Message *message);

    /// @brief Send a message to all connected peer
    void broadcastMessageToAllPeers(Message *message);

    /// @brief Send a message to peers on the UNL (Unique Node List)
    void broadcastMessageToUNLPeers(Message *message);

    /// @brief Add Connection to connectionPool map and ID to setOfConnectedPeers
    /// @return True if success, False if ther is already existing connection with this peer
    bool addConnectionToConnectionPool(const socket_t connectionID, Connection* connection, const std::string &peerID);

    /// @brief Delete Connection from peerConnections map and peerID from serOfConnectedPeers
    void deleteConnectionFromConnectionPool(const socket_t connectionID, const std::string &peerID);

    /// @brief Get a Connection from connectionPool, return nullptr if not found
    Connection* getConnectionFromConnectionPool(const socket_t connectionID);
    /// @brief Get a Connection from connectionPool, return nullptr if not found. THIS METHOD IS NOT OPTIMIZED!
    Connection* getConnectionFromConnectionPool(const std::string &peerID);

    /// @brief Check if exists connection with a peer (peerID). True if yes, False if no
    bool isConnectionInConnectionPool(const std::string &peerID);

    /// @brief Check if connection with given connection ID is in the connectionPool. True if yes, False if no
    bool isConnectionInConnectionPool(const socket_t connectionID);

    /**
     * @brief Update the Peer ID
     * 
     * @param connectionID ID of the connection (socked descriptor of the connection)
     * @param oldPeerID used to be port of the connection with the peer
     * @param newPeerID new ID to be set
     * @return true if operation success
     * @return false if connection with given connectionID is not presented in connection pool
     */
    bool updatePeerIdOfConnectionInConnectionPool(const socket_t connectionID, const std::string &oldPeerID, std::string &newPeerID);

    /// @brief Add new socket descriptor to socketDescriptors vector
    void addSocketDescriptor(const socket_t socket_fd);

    /// @brief Delete the socket descriptor from the socketDescriptors vector
    void deleteSocketDescriptor(const socket_t socket_fd);

    /// @brief Start the server
    bool runServer();

    /// @brief Stop the server
    void stopServer();

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

    /// @brief Method for running ConnectionManager thread
    void manageConnections();

    /// @brief Handle accepting new connections aftere poll() set POLLIN flag
    void handleServerPoll();

    /// @brief Handle message receiving of single connection after poll() set POLLIN flag
    /// @param socket_fd Socket descriptor of the connection
    void handleConnectionPoll(int socket_fd);

    /// @brief Process all requests in the message queue
    void processMessageQueueRequests();

    /// @brief Process all requests in the connection queue
    void processConnectionQueueRequests();
    
};


} // namespace PQB

/* END OF FILE */