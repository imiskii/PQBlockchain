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
#include "PQBModel.hpp"
#include "Signer.hpp"
#include "Log.hpp"


int main(int argc, char *argv[]){

    ArgParser &a = ArgParser::GetInstance(argc, argv);
    args_t parsedArgs = a.getArguments();

    PQB::Log::init();
    PQB::Signer::GetInstance(parsedArgs.signature_alg);

    PQB::PQBModel model(parsedArgs.conf_file_path);
    if (!model.initializeManagers(PQB::NodeType::VALIDATOR)){ // The type is hardcoded to be validator for purpose of testing
        PQB_LOG_ERROR("MAIN", "Failed to initialize managers");
        return 1;
    }
    model.initializeUNLConnections();
    PQB::Console console;
    PQB::Controller controller(&console, &model);
    console.setCommandListener(&controller);

    PQB_LOG_INFO("MAIN", "Application started");
    console.openConsol();
    PQB_LOG_INFO("MAIN", "Aplication closed");

    return 0;
}

/* END OF FILE */