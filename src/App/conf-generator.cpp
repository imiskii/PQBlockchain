/**
 * @file conf-generator.cpp
 * @author Michal Ľaš
 * @brief Generator for PQB wallet configuration files.
 * 
 * This short support program will read lines from stdin and based on this line create configuration files for PQB in tmp/confs/ directory.
 * In the same directory will be also created a file with names of each config file name on separate line.
 * line syntax: <IP address> <balance to set> <line ID of peer who will be added on UNL>
 * One line represents one config file. All fields have to be separated by one space. Line IDs are line number indexed from 1 
 * and they represent lines that will be added for current line to UNL. For example if I have 3 lines:
 * 
 * 10.0.0.1 0 2 3
 * 10.0.0.2 100 1
 * 10.0.0.3 20 1 2
 * 
 * Then the first line will have 2 UNL peers 10.0.0.2 and 10.0.0.3. The second line will have one UNL peer - 10.0.0.1 and last one
 * will have again 2 UNL peers 10.0.0.1 and 10.0.0.2
 * 
 * The program require one argument and it is name of digital signature algorithm that will be used.
 * 
 * @warning There are no checks for given lines or program arguments! Check the outputed configuration files.
 * 
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
#include "Log.hpp"

/// @brief Function for parsing liny by space to a vector
std::vector<std::string> parseLine(std::string &line){
    std::vector<std::string> parsedLine;
    size_t start = 0, end = 0;
    while ((start = line.find_first_not_of(' ', end)) != std::string::npos){
        end = line.find(' ', start);
        parsedLine.push_back(line.substr(start, end - start));
    }
    return parsedLine;
}


int main(int argc, char *argv[]){

    if (argc != 2){
        std::cerr << "Invalid number of arguments!" << std::endl;
        return EXIT_FAILURE;
    }

    PQB::Log::init();
    
    std::string file_path = "tmp/confs/"; // output directory
    PQB::Signer::GetInstance(argv[1]); // set digital signature algorithm

    std::vector<std::string> lines;
    std::vector<PQB::Wallet*> wallets;
    std::string line;
    size_t line_count = 1;

    // File with all configuration paths each on the separate line
    std::ofstream conf("tmp/confs/confs.txt");
    if (!conf.is_open()){
        std::cerr << "Error opening file!" << std::endl;
        return EXIT_FAILURE;
    }

    // First line is used digital signature algorithm
    conf << argv[1] << std::endl;

    // Iterate lines on stdin initialize empty conf*.json files in tmp/confs/ directory
    while (std::getline(std::cin, line)) {
        std::string spec_file_path = file_path + "conf" + std::to_string(line_count) + ".json";
        conf << spec_file_path << std::endl;
        lines.push_back(line);
        PQB::Wallet *wallet = new PQB::Wallet(spec_file_path);
        wallet->loadConfigurationFromFile(); 
        wallets.push_back(wallet);
        line_count++;
    }

    conf.close();

    // For each given line and initialized empty wallet, initialize base on the line the wallet data and generate new pair of keys
    for(size_t i = 0; i < lines.size(); i++){
        PQB::Wallet *wallet = wallets.at(i);
        std::vector<std::string> parsedLine = parseLine(lines.at(i));
        
        // The first item in line is ip address of the peer
        wallet->getAddressList().push_back(parsedLine.at(0));
        // The second item in line is balance of an account
        wallet->setBalance(std::stoul(parsedLine.at(1)));
        // Rest of the line items are line references to peer that will be added as UNL nodes
        for (auto it = parsedLine.begin() + 2; it != parsedLine.end(); ++it){
            wallet->getUNL().push_back(wallets.at(std::stoul(*it) - 1)->getWalletID().getHex());
        }
        wallet->saveConfigurationToFile();
    }

    for (auto wallet : wallets){
        delete wallet;
    }

    return 0;
}