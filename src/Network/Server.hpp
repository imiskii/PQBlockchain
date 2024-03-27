/**
 * @file Server.hpp
 * @author Michal Ľaš
 * @brief Server to accept connections from peers
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <string>
#include <cstring>
#include <thread>
#include <atomic>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "Sock.hpp"
#include "PQBconstants.hpp"


namespace PQB{


/// @brief Representation of the PQB server
class Server{
public:

    Server() : sock(nullptr) {}
    ~Server(){
        CloseServer();
    }

    /// @brief Initialize socket on port 8330 and start listening to connections
    /// @return Initialized socket object
    Sock *OpenServer();

    /// @brief Close the server socket
    void CloseServer();

    /**
     * @brief Accept a connection
     * 
     * @param [out] port This parameter will be set to port number of the new connection
     * @return Sock* Socket object representing new connection
     */
    Sock *AcceptConnection(std::string *port = nullptr);

    /// @brief Get socket descriptor of the server
    /// @return Socket descriptor of the server or -1 if server is not initialized
    int getSocketFD();


private:

    Sock *sock; ///< Server socket object

    /// @brief Initialize the server. Return true on success, false on failure
    bool init();

    /// @brief Set servers socket object (create and bind socket). Return true on success, false on failure
    bool setSocket();
};


} // namespace PQB

/* END OF FILE */