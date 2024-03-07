/**
 * @file Server.cpp
 * @author Michal Ľaš
 * @brief Server to accept connections from peers
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Server.hpp"


namespace PQB{


    void Server::Run(){
        if (init()){
            runAllowed = true;
            serverThread = std::jthread(&Server::serverTask, this);
        }
        /// @todo handle failure
    }

    void Server::Stop(){
        runAllowed = false;
        sock->Shutdown(SHUT_RDWR);
        delete sock;
        serverThread.join();
    }

    bool Server::init(){
        if (!setSocket()){
            return false;
        }
        struct sockaddr_in6 serverAddr;
        std::memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin6_family = AF_INET6;
        serverAddr.sin6_port = htons(PQB_SERVER_PORT);
        serverAddr.sin6_addr = in6addr_any;
        if (sock->Bind((struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0){
            return false;
        }
        if (sock->Listen(MAX_SERVER_QUEUE) < 0){
            return false;
        }
        return true;
    }

    bool Server::setSocket(){
        int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
        if (socket_fd < 0){
            return false;
        }
        sock = new Sock(socket_fd);
        int on = 1;
        if (sock->Setsockopt(SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on))){
            return false;
        }
        return true;
    }

    void Server::serverTask(){
        while (runAllowed)
        {
            struct sockaddr_in6 clientAddr;
            socklen_t clientAddrLen = sizeof(clientAddr);
            std::memset(&clientAddr, 0, clientAddrLen);
            Sock *clientSock = sock->Accept((sockaddr *)&clientAddr, &clientAddrLen);
            if (clientSock == nullptr){
                continue;
            }
            std::string clientPort = std::to_string(ntohs(clientAddr.sin6_port));
            connManager->acceptNewConnection(clientPort, clientSock);
        }
    }

} // namespace PQB

/* END OF FILE */