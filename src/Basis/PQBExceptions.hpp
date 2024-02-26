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
#include <string>

namespace PQB::Exceptions{


class Signer : public std::exception{    
public:
    Signer(const std::string& exceptionMessage) : m_msg(exceptionMessage) {}
    const char *what() const noexcept override{
        return m_msg.c_str();
    }


private:
    const std::string m_msg;
};


class CommandInput : public std::exception{
public:
    CommandInput(const std::string& exceptionMessage) : m_msg(exceptionMessage) {}
    const char* what() const noexcept override{
        return m_msg.c_str();
    }
private:
    const std::string m_msg;
};


class Storage : public std::exception{
public:
    Storage(const std::string& exceptionMessage) : m_msg(exceptionMessage) {}
    const char* what() const noexcept override{
        return m_msg.c_str();
    }
private:
    const std::string m_msg;
};


}


#endif // PQBLOCKCHAIN_EXCEPTIONS_HPP