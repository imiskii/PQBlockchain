
#include <iostream>
#include <string>
#include "Blob.hpp"
#include "HashManager.hpp"


int main(int argc, char *argv[]){

    // ArgParser &a = ArgParser::GetInstance(argc, argv);
    // args_t parsedArgs = a.getArguments();
    
    byte64_t hash1;
    byte64_t hash2;
    byte64_t hash3;

    std::string message = "Hi!";

    PQB::HashMan::SHA512_hash(&hash1, (PQB::byte *) message.data(), message.size());

    std::cout << "Size of byte64_t is: " << sizeof(hash1) << std::endl << hash1.getHex() << std::endl;

    std::memcpy(&hash2, &hash1, sizeof(hash2));
    std::cout << hash2.getHex() << std::endl;

    hash3 = hash2;
    std::cout << hash3.getHex() << std::endl;

    return 0;
}

/* END OF FILE */