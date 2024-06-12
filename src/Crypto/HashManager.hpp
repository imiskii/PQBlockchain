/**
 * @file HashManager.hpp
 * @author Michal Ľaš
 * @brief Interface for SHA-512 hash function
 * @date 2024-02-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <vector>

#include <cryptopp/sha.h>

#include "Blob.hpp"
#include "PQBtypedefs.hpp"


namespace PQB{


class HashMan{
public:

    static constexpr size_t SHA512_SIZE = 64; ///< size of SHA-512 in bytes

    /**
     * @brief Calculates hash of given data
     * 
     * @param result Pointer to result
     * @param inputData Pointer to begining of data
     * @param dataSize Size of the data
     */
    static void SHA512_hash(byte64_t *result, const PQB::byte *inputData, unsigned int dataSize);


    /**
     * @brief Calculates hash of 2 byte64_t together
     * 
     * @param result Pointer to result
     * @param first 
     * @param second 
     */
    static void SHA512_hash(byte64_t *result, const byte64_t &first, const byte64_t &second);
};



} // PQB namespace


/* END OF FILE */