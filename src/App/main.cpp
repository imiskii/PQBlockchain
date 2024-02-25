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
#include "Interface.hpp"
#include "Controller.hpp"
#include "Database.hpp"

int main(int argc, char *argv[]){

    // ArgParser &a = ArgParser::GetInstance(argc, argv);
    // args_t parsedArgs = a.getArguments();

    PQB::Database db = PQB::Database();
    db.openDatabase();
    db.closeDatabase();

    PQB::Console console;
    PQB::Controller controller(&console);
    console.setCommandListener(&controller);
    console.openConsol();

    return 0;
}

/* END OF FILE */