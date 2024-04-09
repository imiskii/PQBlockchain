/**
 * @file Interface.cpp
 * @author Michal Ľaš
 * @brief PQB Console and Listener for commands
 * @date 2024-02-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Interface.hpp"


namespace PQB{


void Console::openConsol(){
    std::string input;

    while (!stopFlag){
        std::cout << ">>> ";
        std::getline(std::cin, input);

        if (std::cin.eof()){
            std::cout << "Consol got EOF, closing." << std::endl;
            break;
        }

        if (std::cin.fail()){
            std::cout << "Input proccessing error!" << std::endl;
            break;
        }

        if (input.empty()){
            continue;
        }

        commandListener->emitCommand(input);
        input.clear();
    }
    std::cout << std::endl;
}

void Console::closeConsol(){
    stopFlag = true;
}

void Console::printErrorToConsole(const char *message) const{
    std::cout << ANSI_COLOR_RED << message << ANSI_COLOR_RESET << std::endl;
}

void Console::printWarningToConsole(const char *message) const{
    std::cout << ANSI_COLOR_YELLOW << message << ANSI_COLOR_RESET << std::endl;
}

void Console::printToConsole(const char *message) const
{
    std::cout << message << std::endl;
}


} // namespace PQB

/* END OF FILE */