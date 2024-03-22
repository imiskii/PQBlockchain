/**
 * @file MessageManagement.hpp
 * @author Michal Ľaš
 * @brief Processor and Checker for PQB net Messages
 * @date 2024-03-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once


#include <string>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <cstring>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <memory>
#include <poll.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Sock.hpp"
#include "Server.hpp"
#include "Message.hpp"
#include "AccountStorage.hpp"
#include "BlocksStorage.hpp"
#include "Connection.hpp"
#include "AccountStorage.hpp"
#include "Wallet.hpp"
#include "Consensus.hpp"


namespace PQB
{

class MessageProcessor;


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

    /// @brief Datatype representing a connection request. Connection requests are mode for ConnectionManager that will try to initialize this connections.
    struct ConnectionRequest_t{
        std::string peerID; ///< wallet ID of peer
        bool setAsUNL;      ///< flag telling if connection should be treated as connection on the UNL
    };

    ConnectionManager(MessageProcessor *msgProcessor, AccountAddressStorage *addressStorage, std::string &walletID);

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
    void addConnectionRequest(ConnectionRequest_t req);

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
    AccountAddressStorage *addrStorage;
    std::string localWalletID; ///< wallet address/peerID of local (this) node
    Server *server; ///< Instance of a PQB server

    std::queue<ConnectionRequest_t> connectionRequestQueue; ///< queue with requests for connection
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
    bool createNewConnection(std::string &peerID, std::vector<std::string> &addresses, bool isOnUNL);

    /**
     * @brief Add connection to connections pool
     * 
     * @param port Port of the connection. Serves as temporory Connection ID
     * @param socket Sock object of the new connection
     * @return true if new connection was created successfuly
     * @return false if failed creating new connection
     */
    bool acceptNewConnection(std::string &port, Sock *socket);

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

    /**
     * @brief Send a message to all connected peer (can exclude on connection). The connectionID and peerID
     * specify connection that will be excluded from BROADCAST. This is useful when some peer send some message that
     * will be propagated further to other peers. In this case it is not useful to send such a message to original sender.
     * 
     * If connectionID is set to 0 and peerID to empty string (""), then this message is sended to all peers without any excludion.
     * 
     * @param connectionID connection ID which will be excluded from broadcast
     * @param peerID peer ID which will be excluded from broadcast
     * @param message message to broadcast
     */
    void broadcastMessageToAllPeers(const socket_t connectionID, std::string &peerID, Message *message);

    /**
     * @brief Send a message to peers on the UNL (Unique Node List). The connectionID and peerID
     * specify connection that will be excluded from UNLCAST. This is useful when some peer send some message that
     * will be propagated further to other peers. In this case it is not useful to send such a message to original sender.
     * 
     * If connectionID is set to 0 and peerID to empty string (""), then this message is sended to all peers without any excludion.
     * 
     * @param connectionID connection ID which will be excluded from unlcast
     * @param peerID peer ID which will be excluded from unlcast
     * @param message message to unlcast
     */
    void broadcastMessageToUNLPeers(const socket_t connectionID, std::string &peerID, Message *message);

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


class MessageProcessor{
public:
    
    MessageProcessor(AccountStorage *accountStorage, BlocksStorage *blockStorage, Consensus *consensusAlgorithm, Wallet *localWallet);
    ~MessageProcessor();

    /// @brief Assign connectionManager to this message processor 
    void assignConnectionManager(ConnectionManager *connectionManager){
        connMng = connectionManager;
    }

    /// @brief Process given message
    void processMessage(int connectionID, std::string &peerID, bool isUNL, Message *message);

    /// @brief Set if Processor should process messages related to consensus
    /// @param onoff true if capture messages related to making consensus, false if ignore these messages
    void setConsensusMessages(bool onoff){
        consensusMessages = onoff;
    }

    /**
     * @brief Check transaction from MessageProcessor perspective. This is checking structure of the transaction,
     * if sender and receiver exist, and if signature on transaction is valid. These are the conditions to process or forward a transaction.
     * 
     * This is not checking the balance or sequence number validity.
     * 
     * @param tx transaction to check
     * @return true if transaction is valid
     * @return false if transaction is invalid
     */
    bool checkTransaction(const TransactionPtr tx);

private:

    ConnectionManager *connMng;
    AccountStorage *accStor;
    BlocksStorage *blockStor;
    Consensus *consensus;
    Wallet *wallet;

    /// @brief Item for processing
    struct message_item_t{
        int connection_id;
        std::string peer_id;
        bool isUNL;
        Message *msg;
    };

    std::jthread processingThread;

    struct ProcessingQueueComparator {
        bool operator()(const message_item_t &a, const message_item_t &b) const {
            return a.msg->getType() < b.msg->getType();
        }
    };

    /// @brief Elements are pairs of connection identifier and message that should be processed
    std::priority_queue<message_item_t, std::vector<message_item_t>, ProcessingQueueComparator> processingQueue;
    std::mutex processingQueueMutex; ///< mutex protecting access to processingQueue
    std::condition_variable processCondition; ///< Condition for processing
    std::atomic_bool processingRun;  ///< flag telling if message processor thred is running

    ///< Map for storing block hashes and information about how many UNL nodes propagates inventory of blocks.
    ///< If UNL quorum for some block is > 80% then the block is considered valid and can be optained from peer with GetData message.
    ///< This mechanism is for Server nodes only because validator nodes make block and consensus so they do not have to relly on other nodes.
    std::map<byte64_t, uint32_t> blockInvQuorum;

    /// @brief Set of data identifier which are awaiting to be recived because GetData message was sent to optain them.
    /// This attribute is used to ensure that just one GetData message is sent
    std::set<byte64_t> waitingData;

    bool consensusMessages; ///< tell if Processor should process also messages related to consensus

    /// @brief Add a message to processing queue
    void addMessageToProcessingQueue(message_item_t &msgi);

    /// @brief Process simple messages that can be answered immediately, other messages put into processingQueue
    /// @return true if early processing was done and late processing is not neccessary, flase if late processing is needed
    bool earlyProcessing(message_item_t &msgi);

    /// @brief Process messages that could not be answered immediately and need more complex processing like access to database etc.
    void lateProcessing(message_item_t &msgi);

    /// @brief Message processor thread
    void messageProcessor();

    /**************************************************************/

    /*
     * "proc" methods are for processing individual messages.
     * each "prco" method can process one specific type of message.
     * The only attribut is message_itme_t structure which contains
     * attributes necessary to create replay on the message and it also
     * contins pointer to received message.
    */

    void procVersionMessage(const message_item_t &msgi);

    void procTransactionMessage(const message_item_t &msgi);

    void procProposalMessage(const message_item_t &msgi);

    void procBlockMessage(const message_item_t &msgi);

    void procAccountMessage(const message_item_t &msgi);

    void procInvMessage(const message_item_t &msgi);

    void procGetDataMessage(const message_item_t &msgi);


    /*
     * Methods for processing individual inventory messages that could be optained from InventoryMessage.
     * Methods return true if this inventory should be optained with GetData message, else they return false
    */

    bool procInvTransaction(const inv_message_t &inv);

    bool procInvBlock(const inv_message_t &inv);

    bool procInvAccount(const inv_message_t &inv);

    /**
     * @brief Send inventory message of a choosen type to other peers
     * 
     * @param tx_id ID of the inventory item
     * @param msgi information about sender of original message (is used to exlude the original sender from broadcast)
     */
    void forwardInvMessage(const byte64_t &item_id, InvType type, const message_item_t &msgi);
    void forwardInvMessage(const inv_message_t &inv, const message_item_t &msgi);


    /*
     * Methods for processing individual GetData messages. Each method takes 2 arguments
     * The first is the id of element to send and the second is inforamtion about the sender of
     * the GetData message so the data could be sended back to him. These methods query local data
     * and send them to peer.
    */

    void procGetTransaction(const byte64_t &tx_id, const message_item_t &msgi);

    void procGetBlock(const byte64_t &block_id, const message_item_t &msgi);

    void procGetAccount(const byte64_t &acc_id, const message_item_t &msgi);

};


} // namespace PQB

/* END OF FILE */

