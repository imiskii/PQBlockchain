/**
 * @file Sock.hpp
 * @author Michal Ľaš
 * @brief Class managing a socket with wrappers for network functions
 * @date 2024-02-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <memory>
#include <sys/socket.h>
#include <unistd.h>


namespace PQB{

typedef int socket_t;
const socket_t INVALID_SOCKET = -1;

/// @brief Manages socket and close it automatically after deletion of the object
class Sock{
public:
    Sock() = delete;
    explicit Sock(socket_t s);
    ~Sock();

    /// @brief Get the socket file descriptor
    socket_t getSocketFD(){
        return socket;
    }

    /// @brief Close the socket
    void Close();

    /// @brief Wrapper for the standard Berkeley sockets bind() function
    int Bind(const struct sockaddr* address, socklen_t address_len) const;

    /// @brief Wrapper for the standard Berkeley sockets listen() function
    int Listen(int backlog) const;

    /// @brief Wrapper for the standard Berkeley sockets accept() function
    Sock *Accept(struct sockaddr* address, socklen_t* address_len) const;

    /// @brief Wrapper for the standard Berkeley sockets connect() function
    int Connect(const struct sockaddr* address, socklen_t address_len) const;

    /// @brief Wrapper for the standard Berkeley sockets send() function
    ssize_t Send(const void* buffer, size_t length, int flags) const;

    /// @brief Wrapper for the standard Berkeley sockets recv() function
    ssize_t Recv(void* buffer, size_t length, int flags) const;

    /// @brief Wrapper for the standard Berkeley sockets setsockopt() function
    int Setsockopt(int level, int option_name, char* option_value, socklen_t option_length) const;

    /// @brief Wrapper for the standard Berkeley sockets getsockopt() function
    int Getsockopt(int level, int option_name, void* option_value, socklen_t* option_length) const;

    /// @brief Wrapper for the standard Berkeley sockets shutdown() function
    int Shutdown(int how);

private:
    socket_t socket;
};



} // namespace PQB

/* END OF FILE */