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

using namespace std;

int main(int argc, char *argv[]){

    ArgParser *a = ArgParser::GetInstance(argc, argv);

    for(int i = 0; i < 10; i++){
        cout << "round: " << i << endl;
    }

    cout << "This is a node" << endl;

    return 0;
}

/* END OF FILE */