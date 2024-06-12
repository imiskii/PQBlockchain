/**
 * @file HexBase.cpp
 * @author Michal Ľaš (xlasmi00@vutbr.cz)
 * @brief Functions for encoding and decoding hexadecimal strings
 * @date 2024-02-14
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "HexBase.hpp"


namespace PQB{

    std::string bytesToHexString(const PQB::byte *byteSource, size_t size){
        std::string hexString;
        hexString.reserve(size*2);
        CryptoPP::HexEncoder encoder(new CryptoPP::StringSink(hexString));
        encoder.Put(byteSource, size);
        encoder.MessageEnd();
        return hexString;
    }


    void hexStringToBytes(const std::string& hexString, PQB::byte *bytes, size_t size){
        CryptoPP::HexDecoder decoder;
        decoder.Put((CryptoPP::byte *) hexString.data(), hexString.size());
        decoder.MessageEnd();
        decoder.Get(bytes, size);
    }
}

/* END OF FILE */