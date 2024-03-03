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

    std::shared_ptr<Server> Server::GetInstance(){
        static std::shared_ptr<Server> instance = nullptr;
        if(instance == nullptr){
            instance = std::make_shared<Server>();
        }
        return instance;
    }

    

} // namespace PQB

/* END OF FILE */