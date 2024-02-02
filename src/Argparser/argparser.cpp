/**
 * @file argparser.cpp
 * @author Michal Ľaš
 * @brief Class for parsing program arguments
 * @date 2024-01-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "argparser.hpp"

using namespace std;


ArgParser::ArgParser(int argc, char **argv): _argCount(argc), _progArgs(argv)
{
    // set arguments options
    this->_short_opt = "h";
    this->_long_opt = {
        {"help", no_argument, nullptr, 'h'},
    };
    // set implicit arguments values
    this->flags = {false};
    // set flag to not parsed arguments yet
    this->__parsed = false;
}

ArgParser *ArgParser::GetInstance(int argc, char **argv)
{
    static ArgParser *__instance = nullptr;
    if(__instance == nullptr){
        __instance = new ArgParser(argc, argv);
    }
    return __instance;
}


args_t ArgParser::getArguments()
{
    // If arguments were already parsed just return them
    if (this->__parsed)
    {
        return this->args;
    }

    int op = 0; // option

    // process program options (arguments)
    while((op = getopt_long(this->_argCount, this->_progArgs, this->_short_opt.c_str(), this->_long_opt.data(), nullptr)) != -1)
    {
        switch (op)
        {
        // help option
        case 'h':
            // single argument '-h' or '--help' is used
            if (this->_argCount == 2) {
                this->__printHelp();
            } else {
                cerr << "no arguments allowed with -h|--help." << endl;
                exit(EXIT_FAILURE);
            }
            break;
        default:
            exit(EXIT_FAILURE); // error message comes from getopt
            break;
        }
    }

    this->__parsed = true;
    return this->args;
}


void ArgParser::__printHelp()
{
    cout << "Print help"
         << endl;
    exit(EXIT_SUCCESS);
}


/* END OF FILE */
