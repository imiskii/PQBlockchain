/**
 * @file Controller.cpp
 * @author Michal Ľaš
 * @brief App Controller, Command factory
 * @date 2024-02-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Controller.hpp"


namespace PQB{

    Controller::~Controller(){
        __deleteCommands();
    }

    void Controller::emitCommand(std::string commandLine) const {
        std::vector<std::string> parsedCommand = __parseCommand(commandLine);
        CommandCreator* command = nullptr;
        try{
            command = __choseCommand(parsedCommand.at(0));
        }
        catch(const PQB::Exceptions::CommandInput& e){
            console->printErrorToConsole(e.what());
            return;
        }
        
        parsedCommand.erase(parsedCommand.begin()); // remove first argument (which should be always command name)

        command->Execute(console, model, parsedCommand);
    }

    std::vector<std::string> Controller::__parseCommand(std::string& commandLine) const{
        std::vector<std::string> parsedCommand;
        size_t start = 0, end = 0;
        while ((start = commandLine.find_first_not_of(' ', end)) != std::string::npos){
            end = commandLine.find(' ', start);
            parsedCommand.push_back(commandLine.substr(start, end - start));
        }
        return parsedCommand;
    }

    CommandCreator* Controller::__choseCommand(std::string& command) const{
        auto it = commandMap.find(command);
        if(it == commandMap.end()){
            std::string err = "Unknow command: '" + command + "'!";
            throw PQB::Exceptions::CommandInput(err.c_str());
        }
        return commandMap.at(command);
    }

    void Controller::__deleteCommands(){
        for (auto &command : commandMap){
            delete command.second;
        }
    }

} // namespace PQB

/* END OF FILE */