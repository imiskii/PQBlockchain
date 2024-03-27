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
#include "PQBconstants.hpp"
#include "Log.hpp"

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
        PQB_LOG_TRACE("NET", "{} message sent to {}", Message::messageTypeToString(message->getType()), connID);
    }

    Message *Connection::receiveMessage(bool *closeFlag){
        *closeFlag = false;
        Message::message_hdr_t header;
        if (!peekForHeader(header, closeFlag))
            return nullptr;

        Message *newMsg = MessageCreator::createMessage(header);
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
        if (newMsg->checkMessage()){
            PQB_LOG_TRACE("NET", "{} message received from {}", Message::messageTypeToString(newMsg->getType()), connID);
            if (filterMessage(newMsg)){
                return newMsg;
            }
        }
        delete newMsg;
        return nullptr;
    }

    bool Connection::peekForHeader(Message::message_hdr_t &header, bool *closeFlag){
        *closeFlag = false;
        byteBuffer buffer;
        size_t offset = 0;
        buffer.resize(Message::getHeaderSize());
        ssize_t nBytes = sock->Recv(buffer.data(), buffer.size(), MSG_PEEK);
        if (nBytes == 0){
            *closeFlag = true;
            return false;
        } else if (nBytes < 0)
            return false;
        Message::deserializeMessageHeader(buffer, offset, header);
        return header.magicNum == MESSAGE_MAGIC_CONST;
    }

    bool Connection::filterMessage(Message *message){
        switch (message->getType())
        {
        case MessageType::ACK:
            isConfirmed = true;
            return false;
        case MessageType::VERSION:
            return !isConfirmed;
        case MessageType::PROPOSAL:
            return (isConfirmed && isUNL);
        default:
            return isConfirmed;
            break;
        }
    }

} // namespace PQB

/* END OF FILE */