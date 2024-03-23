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

    // ArgParser &a = ArgParser::GetInstance(argc, argv);
    // args_t parsedArgs = a.getArguments();
    
    /// @todo have a configuration generation script that will generate config files for N accounts
    /// @todo have a initialization script that will from generated config files initialize account storage 

    // Open database
    // init wallet
    // init MessageProcessor
    // init ConnectionManager + Server
    // init and run consensus or server processor (instead of consensus)
    // run console

    // there won't be need for synchronization because account storage will be initialized and each node will start from genesis block
    // so then there is no need for ACCOUNT, GETACCOUNTS, and GETBLOCKS messages

    // ------------------------------------------------------------------ //

    // Open database and load config
    // init wallet
    // init MessageProcessor, set it to special SYNC (trying for synchronization) mode when it captures only BLOCK, ACCOUNT, INV, VERSION, and ACK messages
    // init ConnectionManager + Server
    // init and run consensus or server processor (instead of consensus)
    // run console

    // if account storage does not include local account then add it here and
    // |-> if account storage is empty wait for manual initialization
    // |-> else init UNL connection and broadcast account message
    // else
    // |-> if account storage is empty wait for manual initialization 
    // |-> else init UNL connection
    // Ask peers to update block storage and account storage
    // |-> after synchronization put Message processor automatically to normall mode

    // ------------------------------------------------------------------ //

    /// @todo Add console command that can add/remove connection from UNL list, if connection already exist just updadte isUNL flag, if it does not
    /// exists create this connection, do not forget to add it to wallet UNL. If it say remove just update isUNL flag if connection 
    /// exists and remove it from wallet UNL connections

    /// @todo Add console command that will initialize connection and update accounts and blocks, this command will also add this connection to UNL

    /// @todo Block updates by block height can be difficult because blocks are stored under block hash key, so to search specific height it would
    /// be needed to iterate and deserialize each block and find the height which is not very optimized. The solution could be to store in the same
    /// level DB also key value pair which will be block heigh and hash to this block. This way block could be searched by block heigh and
    /// return will be block hash which could be queried and processed.

    /// @todo consider using SHA-3 512, currently is used SHA-2 512 and it is also recommended by relevant institution but may be try to use SHA-3

    ArgParser &a = ArgParser::GetInstance(argc, argv);
    args_t parsedArgs = a.getArguments();

    PQB::Log::init();
    PQB::Signer::GetInstance(parsedArgs.signature_alg);

    PQB::PQBModel model(parsedArgs.conf_file_path);
    model.initializeManagers(parsedArgs.node);
    model.initializeUNLConnections();
    PQB::Console console;
    PQB::Controller controller(&console, &model);
    console.setCommandListener(&controller);
    console.openConsol();

    return 0;
}

/* END OF FILE */