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
    const char* msg;
public:
    FirstExample(const char* exceptionMessage) : msg(exceptionMessage) {}
    const char *what() const noexcept override{
        return msg;
    }
};


class SignerEx : public std::exception{    
public:

    const char *what() const noexcept override{
        return m_msg;
    }

    SignerEx(const char* exceptionMessage) : m_msg(exceptionMessage) {}

private:
    const char* m_msg;
};


}


#endif // PQBLOCKCHAIN_EXCEPTIONS_HPP