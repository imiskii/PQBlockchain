/**
 * @file Command.hpp
 * @author Michal Ľaš
 * @brief Commands perforimng PQB actions
 * @date 2024-02-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <vector>
#include <string>
#include "UtilsForPromptCheck.hpp"
#include "Interface.hpp"
#include "PQBModel.hpp"

namespace PQB
{

typedef std::vector<std::string> CommandArgs;


class Command{
public:
    virtual ~Command() {}
    void setArguments(CommandArgs arguments){
        args = arguments;
    }
    void setOutputConsole(Console* console){
        outputConsole = console;
    }

    void setModel(PQBModel *workingModel){
        model = workingModel;
    }
    /// @brief Check given command arguments 
    virtual bool CheckArguments() const = 0;
    /// @brief Execute backend logic behind the command and get result to console
    virtual void Behavior() const = 0;
protected:
    CommandArgs args; ///< Command arguments
    Console *outputConsole;
    PQBModel *model;
};


/// @brief Create new transaction and broadcast it to peers and add it to blockchain
class CreateTxC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
private:
    static const short ARGS_NUM = 2;
};

/// @brief Exit Command
class ExitC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
};

class EchoC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
private:
    static const short ARGS_NUM = 1;
};


class CommandCreator{
public:
    virtual ~CommandCreator(){};
    /// @brief Construct the Command object
    virtual Command* FactoryMethod() const = 0;
    /// @brief Return description of the command 
    virtual const char* getCommandHelp() const = 0;

    /**
     * @brief Execute a command
     * 
     * @param console console where result of command will be printed
     * @param commandArguments arguments of the command
     */
    void Execute(Console* console, PQBModel *model, CommandArgs& commandArguments);
};


/// @brief CreateTx Command creator
class CreateTxCC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new CreateTxC();
    }
    const char* getCommandHelp() const override{
        return "createTx: Create new transaction\n\tcreateTx <amount> <receiver wallet address>";
    }
};

/// @brief Exit Command Creator
class ExitCC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new ExitC();
    }
    const char* getCommandHelp() const override{
        return "exit: Exits the program";
    }
};

class EchoCC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new EchoC();
    }
    const char* getCommandHelp() const override{
        return "echo: Print given statement (it can not include spaces) to console\n\techo <statement>";
    }
};


    
} // namespace PQB


/* END OF FILE */