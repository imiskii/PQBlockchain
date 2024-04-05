/**
 * @file Controller.hpp
 * @author Michal Ľaš
 * @brief App Controller, Command factory
 * @date 2024-02-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <string>
#include <unordered_map>
#include "PQBExceptions.hpp"
#include "Interface.hpp"
#include "PQBModel.hpp"
#include "Command.hpp"

namespace PQB{


class Controller : public CommandListener{
public:
    Controller(Console* commandConsole, PQBModel *workingModel) : console(commandConsole), model(workingModel) {}
    ~Controller();
    
    void emitCommand(std::string commandLine) const override;
    
private:
    Console *console;
    PQBModel *model;

    /**
     * @brief Parse command line by spaces
     * 
     * @param commandLine raw command recived command line
     * @return std::vector<std::string> vector with parsed command arguments
     */
    std::vector<std::string> __parseCommand(std::string& commandLine) const;

    /**
     * @brief Finds creator for given command name
     * 
     * @param command name of command
     * @return CommandCreator* specific Command creator for given command
     * @exception CommandInput if command is not recognized
     */
    CommandCreator* __choseCommand(std::string& command) const;

    /// @brief Delete allocated command creators
    void __deleteCommands();

    /// @brief Map with all existing commands and their creators
    const std::unordered_map<std::string, CommandCreator*> commandMap = {
        {"exit", new ExitCC()},
        {"echo", new EchoCC()},
        {"createTx", new CreateTxCC()},
        {"whoami", new WhoAmICC()},
        {"walletTxs", new PrintWalletTxCC()},
        {"blocks", new PrintBlocksCC()},
        {"blockTxs", new PrintBlockTxsCC()},
        {"accs", new PrintAccountsCC()},
        {"chain", new PrintChainCC()}
    };
};


} // namespace PQB


/* END OF FILE */