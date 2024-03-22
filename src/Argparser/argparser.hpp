/**
 * @file argparser.hpp
 * @author Michal Ľaš
 * @brief Header file for argparse.cpp - Class for parsing program arguments
 * @date 2024-01-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include <getopt.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include "PQBtypedefs.hpp"


#pragma once


using namespace std;


/******************** MACROS ********************/


/****************** DATATYPES *******************/

/**
 * @brief Structure with parsed program arguments
 * 
 */
struct args_t 
{
    PQB::NodeType node;
    std::string signature_alg;
    std::string conf_file_path;
};


/**
 * @brief Argument flags (tells if program argument was given)
 * 
 */
struct arg_flags_t
{
    bool t_flag;    ///< type flag
    bool s_flag;    ///< signature flag
    bool c_flag;    ///< conf flag
    bool h_flag;    ///< help flag
};

/*********** FUNCTIONS AND CLASSES **************/

/**
 * @brief Class representing program arguments and its parsing methods
 * 
 */
class ArgParser
{
protected:
    bool __parsed; ///< Flag telling if program arguments were already parsed

    /**
     * @brief Construct a new Arg Parser object
     *
     * @param argc number of arguments
     * @param argv pointer to array with arguments
     */
    ArgParser(int argc, char **argv);

private:
    /* Private Attributes */

    vector<option> _long_opt;   ///< Long program options
    string _short_opt;          ///< Short program options
    int _argCount;              ///< number of arguments
    char **_progArgs;           ///< program arguments

    /* Private Methods */

    /**
     * @brief print program help info
     *
     */
    void __printHelp();

public:
    ArgParser(ArgParser &other) = delete;
    void operator=(const ArgParser &) = delete;

    /* Public Attributes */
    args_t args;     ///< Parsed program arguments
    arg_flags_t flags;    ///< Program arguments flags telling if the argument was given

    /* Public Methods */

    /**
     * @brief Get the Instance object
     *
     * @param argc number of program arguments
     * @param argv program arguments
     * @return ArgParser& ArgParser class instance
     */
    static ArgParser& GetInstance(int argc, char **argv);

    /**
     * @brief Parse program arguments
     *
     * @return arguments struct with parsed program arguments
     */
    args_t getArguments();
};


/* END OF FILE */
