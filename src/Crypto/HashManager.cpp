/**
 * @file HashManager.cpp
 * @author Michal Ľaš
 * @brief Interface for SHA-512 hash function
 * @date 2024-02-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "HashManager.hpp"

namespace PQB{


void HashMan::SHA512_hash(byte64_t *result, const PQB::byte *inputData, unsigned int dataSize){
    CryptoPP::SHA512 sha512Hash;
    sha512Hash.Update(inputData, dataSize);
    sha512Hash.Final(result->begin());
}


void HashMan::SHA512_hash(byte64_t *result, const byte64_t &first, const byte64_t &second){
    CryptoPP::SHA512 sha512Hash;
    sha512Hash.Update(first.data(), first.size());
    sha512Hash.Update(second.data(), second.size());
    sha512Hash.Final(result->begin());
}

} // PQB namespace


/* END OF FILE */