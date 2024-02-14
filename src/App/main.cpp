/**
 * @file main.cpp
 * @author Michal Ľaš
 * @brief PQBlockchain main
 * @date 2024-01-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <iostream>

#include "argparser.hpp"
#include "Signer.hpp"


int main(int argc, char *argv[]){

    ArgParser &a = ArgParser::GetInstance(argc, argv);
    args_t parsedArgs = a.getArguments();

    std::string message = "Hi, this is a message!";
    PQB::byteBuffer sk;
    PQB::byteBuffer pk;
    PQB::byteBuffer signature;

    PQB::SignAlgorithmPtr signer = PQB::Signer::GetInstance("falcon1024");
    signer->genKeys(sk, pk);

    std::string hexSK = PQB::bytesToHexString(sk.data(), signer->getPrivateKeySize());
    std::string hexPK = PQB::bytesToHexString(pk.data(), signer->getPublicKeySize());
    std::cout << hexSK << std::endl << std::endl;
    std::cout << hexPK << std::endl << std::endl;

    size_t sigSize = signer->sign(signature, (PQB::byte *) message.data(), message.size(), sk);

    std::string hexSignature = PQB::bytesToHexString(signature.data(), sigSize);
    std::cout << hexSignature << std::endl;

    bool result = signer->verify(signature, (PQB::byte *) message.data(), message.size(), pk);

    std::cout << std::endl << "Signature size: " << sigSize << std::endl << "Result: " << result << std::endl;

    return 0;
}

/* END OF FILE */