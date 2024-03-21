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
#include <cstring>
#include <sys/socket.h>
#include "Sock.hpp"
#include "Message.hpp"



namespace PQB{


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
    bool peekForHeader(Message::message_hdr_t &header, bool *closeFlag);

    /// @brief Early filter for messages. Filter all messages except ACK if connection is not confirmed.
    /// If connection is confirmed then filter duplicit VERSION messages (to avoid duplicit VERSION messages).
    /// If connection is not on the node UNL, then filter every message except GET* messages, else process each message. 
    /// @return True if message pass, false if message do not pass
    bool filterMessage(Message *message);

    Sock *sock;
};


} // namespace PQB

/* END OF FILE */