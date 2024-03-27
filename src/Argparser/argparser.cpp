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
#include "PQBconstants.hpp"

using namespace std;


ArgParser::ArgParser(int argc, char **argv): _argCount(argc), _progArgs(argv)
{
    // set arguments options
    this->_short_opt = "ht:s:c:";
    this->_long_opt = {
        {"help", no_argument, nullptr, 'h'},
        {"type", required_argument, nullptr, 't'},
        {"signature", required_argument, nullptr, 's'},
        {"conf", required_argument, nullptr, 'c'},
        {nullptr, 0, nullptr, 0}
    };
    // set implicit arguments values
    this->flags = {false, false, false, false};
    // set flag to not parsed arguments yet
    this->__parsed = false;
}

ArgParser& ArgParser::GetInstance(int argc, char **argv)
{
    static ArgParser __instance(argc,argv);
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
        case 't':
            if (!strcmp(optarg,"server")){
                args.node = PQB::NodeType::SERVER;
            } else if (!strcmp(optarg, "validator")){
                args.node = PQB::NodeType::VALIDATOR;
            } else {
                cerr << "Unknow node type: " << optarg << endl;
                exit(EXIT_FAILURE);
            }
            flags.t_flag = true;
            break;
        case 's':
            if (!strcmp(optarg, "falcon1024") ||
                !strcmp(optarg, "ed25519")){
                    args.signature_alg = optarg;
            } else {
                cerr << "Unknow signature algorithm: " << optarg << endl;
                exit(EXIT_FAILURE);
            }
            flags.s_flag = true;
            break;
        case 'c':
            args.conf_file_path = optarg;
            flags.c_flag = true;
            break;
        default:
            exit(EXIT_FAILURE); // error message comes from getopt
            break;
        }
    }

    if (!flags.s_flag){
        cerr << "Required argument --sig|-s is missing" << endl;
        exit(EXIT_FAILURE);
    }

    if (!flags.t_flag){
        cerr << "Required argument --type|-t is missing" << endl;
        exit(EXIT_FAILURE);
    }

    if (!flags.c_flag) { args.conf_file_path = PQB::CONFIGURATION_FILE; }

    this->__parsed = true;
    return this->args;
}


void ArgParser::__printHelp()
{
    cout << "\nPQ Blockchain required arguments:\n\n"
         << "--type <name of node type> | -t <server/validator>\n\tType of node to be run. Node type could be server or validator.\n"
         << "--sig <signature algorithm> | -s <signature algorithm>\n\tName of signature algorithm to use. For exmaple falcon1024 or ed25519.\n"
         << "\nOptional arguments:\n\n"
         << "--conf <path to wallet configuration file> | -c <path to wallet configuration file>\n\tIf not used the default path is in local directory tmp/conf.json\n\n"
         << "Example of usage:\n"
         << "./main -t validator -s falcon1024\n"
         << "./main -t server -s ed25519 -c tmp/conf1.json\n"
         << endl;
    exit(EXIT_SUCCESS);
}


/* END OF FILE */
