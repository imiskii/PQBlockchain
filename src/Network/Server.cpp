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

    Sock *Server::OpenServer(){
        if (init()){
            return sock;
        }
        /// @todo make log
        if (sock != nullptr){
            delete sock;
            sock = nullptr;
        }
        return nullptr;
    }

    void Server::CloseServer(){
        if (sock != nullptr){
            sock->Shutdown(SHUT_RDWR);
            delete sock;
            sock = nullptr;
        }
    }

    Sock *Server::AcceptConnection(std::string *port){
        if (sock == nullptr){
            return nullptr;
        }
        struct sockaddr_in6 clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        Sock *clientSock = sock->Accept((sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSock == nullptr){
            return nullptr;
        }
        if (port != nullptr){
            std::string clientPort = std::to_string(ntohs(clientAddr.sin6_port));
            *port = clientPort;
        }
        return clientSock;
    }

    int Server::getSocketFD()
    {
        if (sock != nullptr){
            return sock->getSocketFD();
        }
        return -1;
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


} // namespace PQB

/* END OF FILE */