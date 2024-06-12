/**
 * @file Blob.cpp
 * @author Michal Ľaš
 * @brief Implementation of blob and other large datatypes
 * @date 2024-02-10
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Blob.hpp"

template<unsigned int BITS>
std::string base_blob<BITS>::getHex() const{
    return PQB::bytesToHexString(m_data.data(), WIDTH);
}

template<unsigned int BITS>
void base_blob<BITS>::setHex(const std::string &str){
    PQB::hexStringToBytes(str, m_data.begin(), WIDTH);
}

// Explicit instantiations for base_blob<512>
template std::string base_blob<512>::getHex() const;
template void base_blob<512>::setHex(const std::string&);