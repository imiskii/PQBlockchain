/**
 * @file MessageProcessor.cpp
 * @author Michal Ľaš
 * @brief Processor and Checker for PQB net Messages
 * @date 2024-03-01
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "MessageProcessor.hpp"


namespace PQB
{
    MessageProcessor::MessageProcessor(){
        processingThread = std::jthread(&MessageProcessor::messageProcessor, this);
    }

    void MessageProcessor::processMessage(std::string &connectionID, MessageShPtr message){
        // early processing
        // response
        // OR
        // addMessageToProcessingQueue(connectionID, message);
    }

    void MessageProcessor::addMessageToProcessingQueue(std::string &connectionID, MessageShPtr message){
        std::lock_guard<std::mutex> lock(processingQueueMutex);
        processingQueue.emplace(connectionID, message);
    }

    void MessageProcessor::messageProcessor(){
        while (true)
        {
            std::unique_lock<std::mutex> lock(processingQueueMutex);
            processCondition.wait(lock, [this]{ return !processingQueue.empty(); });
            std::pair<std::string, MessageShPtr> msg = processingQueue.front();
            processingQueue.pop();
            lock.unlock();

            //lateProcessing(msg.second);
        }
    }


} // namespace PQB

/* END OF FILE */
