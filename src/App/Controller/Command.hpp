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
#include <sstream>
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

/// @brief Echo command -> print given argument
class EchoC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
private:
    static const short ARGS_NUM = 1;
};

/// @brief Print id/wallet address of this node
class WhoAmIC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
};

/// @brief Print transactions made with local wallet
class PrintWalletTxC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
};

/// @brief Print Blocks stored in blocks storage
class PrintBlocksC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
};

/// @brief Print Transactions of given block
class PrintBlockTxsC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
private:
    static const short ARGS_NUM = 1;
};

/// @brief Print Accounts stored in account storage
class PrintAccountsC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
};

/// @brief Print tree of blocks from geneseis block to most recent block
class PrintChainC : public Command{
public:
    bool CheckArguments() const override;
    void Behavior() const override;
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

/// @brief Echo Command Creator
class EchoCC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new EchoC();
    }
    const char* getCommandHelp() const override{
        return "echo: Print given statement (it can not include spaces) to console\n\techo <statement>";
    }
};

/// @brief WhoAmI Comand Creator
class WhoAmICC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new WhoAmIC();
    }
    const char* getCommandHelp() const override{
        return "whoami: Print local wallet identifier";
    }
};

/// @brief PrintWalletTxC Comand Creator
class PrintWalletTxCC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new PrintWalletTxC();
    }
    const char* getCommandHelp() const override{
        return "walletTxs: Print transactions made with local wallet or received by this wallet";
    }
};

/// @brief PrintBlocksC Comand Creator
class PrintBlocksCC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new PrintBlocksC();
    }
    const char* getCommandHelp() const override{
        return "blocks: Print Blocks stored in blocks storage database";
    }
};

/// @brief PrintBlockTxsC Comand Creator
class PrintBlockTxsCC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new PrintBlockTxsC();
    }
    const char* getCommandHelp() const override{
        return "blockTxs: Print Transactions in given block\n\tblockTxs <block ID>";
    }
};

/// @brief PrintAccountsC Comand Creator
class PrintAccountsCC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new PrintAccountsC();
    }
    const char* getCommandHelp() const override{
        return "accs: Print Accounts stored in account storage database";
    }
};

/// @brief PrintChainC Comand Creator
class PrintChainCC : public CommandCreator{
public:
    Command* FactoryMethod() const override{
        return new PrintChainC();
    }
    const char* getCommandHelp() const override{
        return "chain: Print tree of blocks from geneseis block to most recent block";
    }
};

    
} // namespace PQB


/* END OF FILE */