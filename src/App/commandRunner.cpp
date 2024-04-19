/**
 * @file commandRunner.cpp
 * @author Michal Ľaš
 * @brief Script for automatic command printing based on given node with time delays
 * 
 * The script should be run with 2 arguments:
 * 1. path to file with a command script *
 * 2. name of a node for which the commands should be executed
 * 
 * * Command script is a .txt file on each line it has same command for pqb application. It can also have a command sleep <amount> which
 * will delay command executing by amount miliseconds. Example:
 * 
 * node1 createTx 10 25AC69DDEF7548FD5D....
 * node3 createTx 10 25AC69DDEF7548FD5D....
 * sleep 1500
 * node2 createTx 10 25AC69DDEF7548FD5D....
 *  .
 *  .
 *  .
 * node1 exit
 * node2 exit
 * node3 exit
 * 
 * @date 2024-04-19
 * 
 * @warning This script does not check provided arguments! Also, the all commands in command script file have to be well formated.
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <string>
#include <utility>
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

/// @brief Function for parsing line to node name and command
std::pair<std::string,std::string> parseLine(std::string &line){
    std::pair<std::string,std::string> parsedLine;
    size_t frst_space = line.find(' ');
    parsedLine.first = line.substr(0, (frst_space));
    parsedLine.second = line.substr((frst_space + 1));
    return parsedLine;
}

int main(int argc, char *argv[]) {

    if (argc != 3){
        std::cerr << "Invalid number of arguments!" << std::endl;
        return EXIT_FAILURE;
    }

    std::string node_name = argv[2];    // second argument should be name of a node
    int default_delay = 500;            // default delay is 500ms

    // Open file with commands
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file with commands on path: " << argv[1] << std::endl;
        return EXIT_FAILURE;
    }

    // Read commands
    std::string line;
    while (std::getline(file, line)) {
        std::pair<std::string,std::string> cmd = parseLine(line);
        if (cmd.first == node_name){
            std::cout << cmd.second << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(default_delay));
        } else if (cmd.first == "sleep"){
            std::this_thread::sleep_for(std::chrono::milliseconds(std::stoul(cmd.second, nullptr)));
        }
    }

    // Close file
    file.close();

    return 0;
}
