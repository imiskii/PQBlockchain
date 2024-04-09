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
        return args.empty();
    }

    void ExitC::Behavior() const{
        outputConsole->closeConsol();
    }

    bool EchoC::CheckArguments() const{
        return (args.size() == ARGS_NUM ? true : false);
    }

    void EchoC::Behavior() const{
        outputConsole->printToConsole(args.at(0).c_str());
    }

    bool CreateTxC::CheckArguments() const{
        if (args.size() == ARGS_NUM){
            // check if first argument is a number
            // second argument have to be 128 long hexadecimal string representing wallet address
            if (isNumber(args.at(0)) && (args.at(1).size() == 128) && isHexadecimal(args.at(1))){
                return true;
            }
        }
        return false;
    }

    void CreateTxC::Behavior() const{
        uint32_t amount = std::stoul(args.at(0));
        std::string receiver = args.at(1);
        std::string ret = model->createTransaction(receiver, amount);
        outputConsole->printToConsole(ret.c_str());
    }

    bool WhoAmIC::CheckArguments() const{
        return args.empty();
    }

    void WhoAmIC::Behavior() const{
        outputConsole->printToConsole(model->getLocalWalletId().data());
    }

    bool PrintWalletTxC::CheckArguments() const{
        return args.empty();
    }

    void PrintWalletTxC::Behavior() const{
        std::stringstream ss;
        model->getDataToPrint(ss, PQB::PQBModel::PrintData::WALLET_TXs);
        outputConsole->printToConsole(ss.str().c_str());
    }

    bool PrintBlocksC::CheckArguments() const{
        return args.empty();
    }

    void PrintBlocksC::Behavior() const{
        std::stringstream ss;
        model->getDataToPrint(ss, PQB::PQBModel::PrintData::BLOCKS);
        outputConsole->printToConsole(ss.str().c_str());
    }

    bool PrintBlockTxsC::CheckArguments() const{
        if (args.size() == ARGS_NUM){
            return isHexadecimal(args.at(0));
        }
        return false;
    }

    void PrintBlockTxsC::Behavior() const{
        std::stringstream ss;
        model->getDataToPrint(ss, PQB::PQBModel::PrintData::BLOCK_TXs, args.at(0));
        outputConsole->printToConsole(ss.str().c_str());
    }

    bool PrintAccountsC::CheckArguments() const{
        return args.empty();
    }

    void PrintAccountsC::Behavior() const{
        std::stringstream ss;
        model->getDataToPrint(ss, PQB::PQBModel::PrintData::ACCOUNTS);
        outputConsole->printToConsole(ss.str().c_str());
    }

    bool PrintChainC::CheckArguments() const{
        return args.empty();
    }

    void PrintChainC::Behavior() const{
        std::stringstream ss;
        model->getDataToPrint(ss, PQB::PQBModel::PrintData::CHAIN);
        outputConsole->printToConsole(ss.str().c_str());
    }

    bool PrintConnectionsC::CheckArguments() const{
        return args.empty();
    }

    void PrintConnectionsC::Behavior() const{
        std::stringstream ss;
        model->getDataToPrint(ss, PQB::PQBModel::PrintData::CONNECTIONS);
        outputConsole->printToConsole(ss.str().c_str());
    }

} // namespace PQB

/* END OF FILE */