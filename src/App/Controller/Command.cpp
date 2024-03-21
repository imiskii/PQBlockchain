/**
 * @file Command.cpp
 * @author Michal Ľaš
 * @brief Commands perforimng PQB actions
 * @date 2024-02-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Command.hpp"


namespace PQB{

    void CommandCreator::Execute(Console* console, PQBModel *model, CommandArgs& commandArguments){
        Command* cmd = this->FactoryMethod();
        cmd->setOutputConsole(console);
        cmd->setModel(model);
        cmd->setArguments(commandArguments);
        if (cmd->CheckArguments()){
            cmd->Behavior();
        } else{
            console->printErrorToConsole("Invalid command arguments!");
            console->printWarningToConsole(this->getCommandHelp());
        }
        delete cmd;
    }

    bool ExitC::CheckArguments() const{
        return (args.empty() ? true : false);
    }

    void ExitC::Behavior() const{
        /// @todo: exit actions
        outputConsole->closeConsol();
    }

    bool EchoC::CheckArguments() const{
        return (args.size() == ARGS_NUM ? true : false);
    }

    void EchoC::Behavior() const{
        outputConsole->printToConsole(args.at(0).c_str());
    }

} // namespace PQB

/* END OF FILE */