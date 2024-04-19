/**
 * @file acc-generator.cpp
 * @author Michal Ľaš
 * @brief This short program fills PQB Account Database with account from given conf*.json files.
 * Program is awaiting input on stdin in format of paths to the conf*.json files. The first input have to be the name of 
 * used digital signature algorithm.
 * 
 * For example:
 * 
 * ed25519
 * tmp/confs/conf1.json
 * tmp/confs/conf2.json
 * 
 * These 3 lines will be processed that first line is the name of used digital signature algorithm to use. And then there are
 * 2 config files which will be taken and their data will be put into the PQB account database.
 * 
 * @warning There is no checks for given input to the program. 
 * 
 * @date 2024-03-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstddef>
#include "Wallet.hpp"
#include "Signer.hpp"
#include "AccountStorage.hpp"
#include "Log.hpp"


int main(){

    PQB::Log::init();

    // Load all inputed files (the first is the digital signature algorithm)
    std::vector<std::string> files;
    std::string file;
    while (std::getline(std::cin, file)) {
        files.push_back(file.substr(0, file.find(' ')));
    }

    // Initialize Signer with this name of signature algorithm
    PQB::Signer::GetInstance(files.at(0));
    files.erase(files.begin());

    // Iterate given file paths load them, create an Account object and put it into the database
    PQB::AccountStorage accStorage;
    accStorage.openDatabases();
    for (auto& file : files) {
        PQB::Wallet wallet(file);
        if (!wallet.loadConfigurationFromFile()){
            std::cerr << "Error: file: " << file << " faild to open!" << std::endl;
            continue;
        }
        if (wallet.getWalletID().IsNull()){
            continue;
        }
        PQB::Account acc; 
        acc.publicKey =  wallet.getPublicKey();
        acc.balance = wallet.getBalance();
        acc.txSequence = wallet.getTxSeqNum();
        acc.addresses = wallet.getAddressList();
        byte64_t acc_id = acc.getAccountID();

        if (!accStorage.setAccount(acc_id, acc)){
            std::cerr << "Failed to put account in file: " << file << " into the database!" << std::endl;
        }
    }

    return 0;
}