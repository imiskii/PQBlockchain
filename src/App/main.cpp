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

#include "Database.hpp"
#include "argparser.hpp"
#include "Interface.hpp"
#include "Controller.hpp"

int main(int argc, char *argv[]){

    // ArgParser &a = ArgParser::GetInstance(argc, argv);
    // args_t parsedArgs = a.getArguments();

    PQB::AccountBalancesStorage db = PQB::AccountBalancesStorage();
    try{
        db.openDatabase();
    }
    catch(const PQB::Exceptions::Storage& e){
        std::cerr << e.what() << '\n';
    }
    
    
    PQB::Console console;
    PQB::Controller controller(&console);
    console.setCommandListener(&controller);
    console.openConsol();

    return 0;
}

/* END OF FILE */