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

    Connection::Connection(Sock *socket, Responder * messageResponder) : sock(socket), responder(messageResponder), connected(true){
        responder->assignToConnection(this);
        readerThread = std::jthread(messageReader);
        senderThread = std::jthread(messageSender);
    }

    Connection::~Connection(){
        // Threads are stopped automatically
        responder->revokeFromConnection();
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
            char buffer[bufferSize];
            std::memset(buffer, 0, bufferSize);

            /// @todo There is not timeout on receiving one large message. If second part of the message is larger than MAX_MESSAGE_SIZE and it does not come the thread will be stuck here
            while (bytesReceived != newMsg->getSize()){
                size_t nBytes = sock->Recv(buffer, bufferSize, 0);
                if (nBytes <= 0)
                    break;
                
                newMsg->addFragment(buffer, nBytes);
                bytesReceived += nBytes;
            }
            if (bytesReceived == newMsg->getSize() && newMsg->checkMessage() && responder)
                responder->processMessage(newMsg); // send message to processing
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
            size_t bytesToSend = msg->getSize();
            size_t bytesSended = 0;
            while (bytesToSend > 0){
                size_t nBytes = sock->Send(msg->getData() + bytesSended, (bytesToSend < MAX_MESSAGE_SIZE ? bytesToSend : MAX_MESSAGE_SIZE), 0);
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
        /// @todo notify Connection Manager
    }

    bool Connection::peekForHeader(Message::message_hdr_t *header){
        size_t nBytes = sock->Recv(header, sizeof(struct Message::message_hdr_t), MSG_PEEK | MSG_WAITALL);
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

    void ConnectionManager::createNewConnection(){
        Responder *responder = messsagProcessor->createResponder();
        socket_t socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
        Sock *sock = new Sock(socketDescriptor);
        Connection *connection = new Connection(sock, responder);
    }

} // namespace PQB

/* END OF FILE */