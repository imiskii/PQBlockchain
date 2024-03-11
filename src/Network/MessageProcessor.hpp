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
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Message.hpp"


namespace PQB
{


class MessageProcessor{
public:
    
    MessageProcessor();

    /// @brief Process given message
    void processMessage(int connectionID, std::string &peerID, Message *message);

    /// @brief Set if Processor should process messages related to consensus
    /// @param onoff true if capture messages related to making consensus, false if ignore these messages
    void setConsensusMessages(bool onoff){
        consensusMessages = onoff;
    }

private:

    std::jthread processingThread;

    /// @todo priority queue!
    std::queue<std::pair<std::string, Message*>> processingQueue; ///< Elements are pairs of connection identifier and message that should be processed
    std::mutex processingQueueMutex; ///< mutex protecting access to processingQueue
    std::condition_variable processCondition; ///< Condition for processing

    bool consensusMessages; ///< tell if Processor should process also messages related to consensus

    /// @brief Add a message to processing queue
    void addMessageToProcessingQueue(std::string &connectionID, Message *message);

    /// @brief Process simple messages that can be answered immediately, other messages put into processingQueue
    //void earlyProcessing();

    /// @brief Process messages that could not be answered immediately and need more complex processing
    //void lateProcessing();

    /// @brief Message processor thread
    void messageProcessor();
};


} // namespace PQB

/* END OF FILE */

