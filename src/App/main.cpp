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
#include <PQBlockchainExceptions.hpp>


int main(int argc, char *argv[]){

    ArgParser *a = ArgParser::GetInstance(argc, argv);

    for(int i = 0; i < 10; i++){
        std::cout << "round: " << i << std::endl;
    }

    std::cout << "This is a node" << std::endl;

    throw PQB::Exceptions::FirstExample("Hello exception");

    return 0;
}

/* END OF FILE */