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
#include "PQBconstatnts.hpp"


namespace PQB{


class Server{
public:

    Server() : sock(nullptr) {}
    ~Server(){
        CloseServer();
    }

    Sock *OpenServer();

    void CloseServer();

    /**
     * @brief Accept a connection
     * 
     * @param [out] port This parameter will be set to port number of the new connection
     * @return Sock* Socket object representing new connection
     */
    Sock *AcceptConnection(std::string *port = nullptr);


    Sock *getSock(){
        return sock;
    }

    int getSocketFD();


private:

    Sock *sock;

    bool init();

    bool setSocket();
};


} // namespace PQB

/* END OF FILE */