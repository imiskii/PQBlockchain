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
public:
    FirstExample(const char* exceptionMessage) : m_msg(exceptionMessage) {}
    const char *what() const noexcept override{
        return m_msg;
    }
private:
    const char* m_msg;
};


class SignerEx : public std::exception{    
public:
    SignerEx(const char* exceptionMessage) : m_msg(exceptionMessage) {}
    const char *what() const noexcept override{
        return m_msg;
    }


private:
    const char* m_msg;
};


class CommandInputEx : public std::exception{
public:
    CommandInputEx(const char* exceptionMessage) : m_msg(exceptionMessage) {}
    const char* what() const noexcept override{
        return m_msg;
    }
private:
    const char* m_msg;
};


class Storage : public std::exception{
public:
    Storage(const char* exceptionMessage) : m_msg(exceptionMessage) {}
    const char* what() const noexcept override{
        return m_msg;
    }
private:
    const char* m_msg;
};


}


#endif // PQBLOCKCHAIN_EXCEPTIONS_HPP