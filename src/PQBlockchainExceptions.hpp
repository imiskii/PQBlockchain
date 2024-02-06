/**
 * @file exceptions.hpp
 * @author Michal Ľaš
 * @brief custom exceptions for PQBlockchain project
 * @date 2024-02-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#ifndef PQBLOCKCHAIN_EXCEPTIONS_HPP
#define PQBLOCKCHAIN_EXCEPTIONS_HPP

#include <iostream>

namespace PQB::Exceptions{

    class FirstExample : public std::exception{
        private:
            char *msg;
        public:
            FirstExample(char *exceptionMessage) : msg(exceptionMessage) {}
            char *what(){
                return this->msg;
            }
    };
}


#endif // PQBLOCKCHAIN_EXCEPTIONS_HPP