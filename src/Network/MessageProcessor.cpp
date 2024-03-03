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
    void Responder::assignToConnection(Connection *conn){
        std::lock_guard<std::mutex> lock(connectionMutex);
        connection = conn;
    }

    void Responder::revokeFromConnection(){
        std::lock_guard<std::mutex> lock(connectionMutex);
        connection = nullptr;
        unregisterFromMessageProcessor();
    }

    bool Responder::isConnected(){
        std::lock_guard<std::mutex> lock(connectionMutex);
        return connection != nullptr;
    }

    void Responder::sendMessage(MessageShPtr message){
        std::lock_guard<std::mutex> lock(connectionMutex);
        if (connection != nullptr){
            connection.sendMessage(message);
        }
    }

    void Responder::registerToMessageProcessor() const{
        messageProcessor->registerResponder(this, ID);
    }

    void Responder::unregisterFromMessageProcessor() const{
        messageProcessor->unregisterResponder(ID);
    }

    void Responder::processMessage(MessageShPtr message){
        
    }

/************************************************************************/
/**************************** MessageProcessor **************************/
/************************************************************************/

    MessageProcessor::MessageProcessor(){
        processingThread = std::jthread(messageProcessor);
    }

    void MessageProcessor::addMessageToProcessingQueue(uint32_t responderID, MessageShPtr message){
        std::lock_guard<std::mutex> lock(processingQueueMutex);
        processingQueue.emplace(responderID, message);
    }

    Responder *MessageProcessor::createResponder(){        
        return new Responder(this);
    }

    void MessageProcessor::registerResponder(const Responder *responder, const std::string &ID){
        std::lock_guard<std::mutex> lock(responderPoolMutex);
        respondersPool.insert({ID, responder});
    }

    void MessageProcessor::unregisterResponder(const std::string &ID){
        std::lock_guard<std::mutex> lock(responderPoolMutex);
        respondersPool.erase(ID);
    }

    void MessageProcessor::messageProcessor(){
        while (true)
        {
            std::unique_lock<std::mutex> lock(processingQueueMutex);
            processCondition.wait(lock, [this]{ return !processingQueue.empty(); });
            std::pair<std::string, MessageShPtr> msg = processingQueue.front();
            processingQueue.pop();
            lock.unlock();

            processMessage(msg.second);
        }
    }

    void MessageProcessor::processMessage(MessageShPtr message){

    }

} // namespace PQB

/* END OF FILE */
