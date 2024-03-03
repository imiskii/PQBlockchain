/**
 * @file MessageProcessor.hpp
 * @author Michal Ľaš
 * @brief Processor and Checker for PQB net Messages
 * @date 2024-03-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once


#include <string>
#include <queue>
#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Message.hpp"


namespace PQB
{


class Connection;
class MessageProcessor;


/// @brief Responder is in first line when it comes to answering messages that comes from a Connection.
/// Simple messages that can be answered immediately are processed. Other messages are delegated to MessageProcessor,
/// which is processing messages from multiple Responders/Connections.
/// Responder has to be created with MessageProcessor object, then assigned to a Connection. If a Conenction is initialized by
/// this node ID/address of peer to which is connected has to be also set (see setResponderID). Otherwise the ID is obtained from Version message.
class Responder{
public:

    std::string ID; ///< Responder ID (ID of connected peer)

    /// @brief Message Responder. Needs to be assigned to a Connection
    /// @param msgProcessor Pointer to instance of Message Processor to which this Responder will be assigned
    Responder(MessageProcessor *msgProcessor) : messageProcessor(msgProcessor), connection(nullptr), ID("") {}

    /// @brief [called from the Connection] Assign this Responder to an existing Connection
    void assignToConnection(Connection *conn);

    /// @brief [called from the Connection] Set ID of peer that is connected to Connection which is assigned to this Responder
    void setResponderID(std::string &peerID){
        ID = peerID;
    }

    /// @brief [called from the Connection] Revoke assignment from the Connection
    void revokeFromConnection();

    /// @brief [called from the Message Processor] true if connected to a Connection, false if not
    bool isConnected();

    /// @brief [called from the Connection] Process captured message on the Connection. If the Message needs additional processing delegate it to Message Processor
    void processMessage(MessageShPtr message);

    /// @brief [called from the Message Processor] Send message to Connection, so it can be sent to peer
    void sendMessage(MessageShPtr message);

private:
    MessageProcessor *messageProcessor;
    Connection *connection;    ///< Connection may close. If connection is closed then it is set to nullptr (by connection thread)
    std::mutex connectionMutex; ///< Connection is accessed by 2 threads so it has to be protected by mutex (Message processing thread is accessing it for message sending and Connection is accessing it for assigning it and revoking assignment)

    /// @brief [call the Message Processor] Once is Register assigned to a Connection and Version/VerAck message was received, then the Responder can be registered to its Message Processor 
    void registerToMessageProcessor() const;

    /// @brief [call the Message Processor] If connection was closed, then unregister this Responder from Message Processor
    void unregisterFromMessageProcessor() const;
};


class MessageProcessor{
public:
    
    MessageProcessor();

    /// @brief Creates new Responder assigned to this MessageProcessor
    Responder* createResponder();

    /// @brief [called from a Responder] Add a message to processing queue
    void addMessageToProcessingQueue(uint32_t responderID, MessageShPtr message);

    /// @brief [called from a Responder] Add Responder to respondersPool with given identifier. This means that Responder was
    /// successfuly assigned to a Connection and Version/VerAck message was received.
    void registerResponder(const Responder *responder, const std::string &ID);

    /// @brief [called from a Responder] Delete existing Responder. Messages from this Responder in processingQueue will be ignored.
    void unregisterResponder(const std::string &ID);

    /// @brief Set if Processor should process messages related to consensus
    /// @param onoff true if capture messages related to making consensus, false if ignore these messages
    void setConsensusMessages(bool onoff){
        consensusMessages = onoff;
    }

private:

    std::jthread processingThread;

    std::unordered_map<std::string, const Responder *> respondersPool; ///< pool of responders <ID, pointer to responder>
    std::mutex responderPoolMutex; ///< mutex protecting access to repondersPool

    /// @todo priority queue!
    std::queue<std::pair<std::string, MessageShPtr>> processingQueue;
    std::mutex processingQueueMutex; ///< mutex protecting access to processingQueue
    std::condition_variable processCondition; ///< Condition for processing

    bool consensusMessages; ///< tell if Processor should process also messages related to consensus

    /// @brief Main processing of a message
    void processMessage(MessageShPtr message);

    /// @brief Message processor thread
    void messageProcessor();
};


} // namespace PQB

/* END OF FILE */

