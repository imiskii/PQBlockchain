/**
 * @file Interface.hpp
 * @author Michal Ľaš
 * @brief PQB Console and Listener for commands
 * @date 2024-02-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <iostream>
#include <string>
#include <ncurses.h>


// ANSI escape codes for text colors
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_WHITE   "\x1b[37m"
#define ANSI_COLOR_RESET   "\x1b[0m"


namespace PQB{


class CommandListener{
public:
    virtual void emitCommand(std::string command) const = 0;
};


class Console{
public:
    Console(){
        stopFlag = false;
    }
    ~Console() {}

    void openConsol();
    void closeConsol();
    void setCommandListener(CommandListener* listener){
        commandListener = listener;
    }

    void printErrorToConsole(const char* message) const;
    void printWarningToConsole(const char* message) const;
    void printToConsole(const char* message) const;

private:
    bool stopFlag;  ///< true if consol should stop
    CommandListener* commandListener;
};



} // namespace PQB



/* END OF FILE */