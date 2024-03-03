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

#include <memory>


namespace PQB{


class Server{
public:
    Server() = delete;
    void operator=(const Server &) = delete;
    static std::shared_ptr<Server> GetInstance();

    void Run();

    void Stop();


private:
    void init();
    bool isRunning;
};


} // namespace PQB

/* END OF FILE */