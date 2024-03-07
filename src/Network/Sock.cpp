/**
 * @file Sock.cpp
 * @author Michal Ľaš
 * @brief Class managing a socket with wrappers for network functions
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Sock.hpp"


namespace PQB{

    Sock::Sock(socket_t s) : socket(s) {}

    Sock::~Sock(){
        Close();
    }

    void Sock::Close(){
        if(socket == INVALID_SOCKET)
            return;
        
        int ret = close(socket);
        if (ret)
            /// @todo make log
            
        socket = INVALID_SOCKET;
    }

    int Sock::Bind(const sockaddr *address, socklen_t address_len) const{
        return bind(socket, address, address_len);
    }

    int Sock::Listen(int backlog) const{
        return listen(socket, backlog);
    }

    Sock *Sock::Accept(sockaddr *address, socklen_t *address_len) const{
        Sock *sock = nullptr;
        int newSocket = accept(socket, address, address_len);
        if(newSocket >= 0){
            sock = new Sock(newSocket);
        }
        return sock;
    }

    int Sock::Connect(const sockaddr *address, socklen_t address_len) const{
        return connect(socket ,address, address_len);
    }

    ssize_t Sock::Send(const void *buffer, size_t length, int flags) const{
        return send(socket, buffer, length, flags);
    }

    ssize_t Sock::Recv(void *buffer, size_t length, int flags) const{
        return recv(socket, buffer, length, flags);
    }

    int Sock::Setsockopt(int level, int option_name, char *option_value, socklen_t option_length) const{
        return setsockopt(socket, level, option_name, option_value, option_length);
    }

    int Sock::Getsockopt(int level, int option_name, void *option_value, socklen_t *option_length) const{
        return getsockopt(socket, level, option_name, option_value, option_length);
    }

    int Sock::Shutdown(int how){
        return shutdown(socket, how);
    }

} // namespace PQB

/* END OF FILE */