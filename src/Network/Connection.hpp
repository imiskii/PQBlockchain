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

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <cstring>
#include "Sock.hpp"
#include "Message.hpp"
#include "MessageProcessor.hpp"


namespace PQB{


/**
 * @brief 
 * 
 * Each Connection has 2 threads first for receiving messages and second for sending messages to a socekt
 * For sending messages there is a message queue. Received messages are delegated to Checker and then to MessageProcessor.
 * 
 */
class Connection{
public:

    Connection(Sock *socket, Responder *messageResponder);
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
    Responder *responder;
};


class ConnectionManager{
public:

    ConnectionManager(MessageProcessor *msgProcessor) : messsagProcessor(msgProcessor) {}
    ~ConnectionManager() {}


    // create socket
    // create responder
    // 
    void createNewConnection();

    void initUNLConnection();

private:
    MessageProcessor *messsagProcessor;
    // some connection to DB (UNL)
};










} // namespace PQB

/* END OF FILE */