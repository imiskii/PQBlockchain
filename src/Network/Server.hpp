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
#include "Connection.hpp"
#include "Sock.hpp"
#include "PQBconstatnts.hpp"


namespace PQB{


class Server{
public:

    Server(ConnectionManager *connectionManager) : connManager(connectionManager), sock(nullptr), runAllowed(false) {}
    ~Server(){
        delete sock;
    }

    void Run();

    void Stop();


private:

    ConnectionManager *connManager;
    Sock *sock;

    std::jthread serverThread;
    std::atomic_bool runAllowed;

    bool init();

    bool setSocket();

    void serverTask();
};


} // namespace PQB

/* END OF FILE */