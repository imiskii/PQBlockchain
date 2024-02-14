/**
 * @file HexBase.hpp
 * @author Michal Ľaš
 * @brief Functions for encoding and decoding hexadecimal strings
 * @date 2024-02-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <string>
#include <cryptopp/hex.h>       // HexEncoder
#include <cryptopp/simple.h>    // StringSink
#include "PQBtypedefs.hpp"

namespace PQB{

/**
 * @brief Transform bytes to hexadecimal string
 * 
 * @param byteSource pointer to bytes
 * @param size size of bytes
 * @return std::string hexadecimal representation of bytes
 */
std::string bytesToHexString(const PQB::byte* byteSource, size_t size);


/**
 * @brief Tronsform hexadecimal string to bytes
 * 
 * @param hexString hexadecimal string
 * @param bytes pointer to bytes
 * @param size size of bytes (have to be at least hexString.size() / 2)
 */
void hexStringToBytes(const std::string& hexString, PQB::byte* bytes, size_t size);


}


/* END OF FILE */