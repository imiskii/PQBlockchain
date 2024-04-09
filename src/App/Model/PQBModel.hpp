/**
 * @file PQBModel.hpp
 * @author Michal Ľaš
 * @brief Functions/Methods for operations with PQ Blockchain
 * @date 2024-03-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <string>
#include "PQBconstants.hpp"
#include "Wallet.hpp"
#include "BlocksStorage.hpp"
#include "AccountStorage.hpp"
#include "MessageManagement.hpp"
#include "Chain.hpp"
#include "Consensus.hpp"

namespace PQB{

class PQBModel{
private:
    Wallet *wallet;
    BlocksStorage *blockS;
    AccountStorage *accS;
    ConnectionManager *connMng;
    MessageProcessor *msgPrc;
    ConsensusWrapper *consensus;
    Chain *chain;

    /// @brief Trys to open all configuration files and databases needed for PQB. If operation success return true, else return false
    bool openConfigurationAndDatabase();

public:

    PQBModel(std::string &config_file_path);
    ~PQBModel();

    /// @brief Trys to initialize ConnectionManager, MessageProcessor and Consensus()
    /// @return true if operation is successful false if operation fails
    bool initializeManagers(NodeType node_type);

    /// @brief Initialize connections on Unique Node List
    void initializeUNLConnections();

    /**
     * @brief Create new Transaction broadcast it to peers
     * 
     * @param receiver wallet address of the receiver
     * @param amount amount of resources to transfer
     * @return std::string status of this operation
     */
    std::string createTransaction(std::string &receiver, PQB::cash amount);

    /// @brief Get hexadecimal representation of local wallet identifier
    std::string_view getLocalWalletId();

    /// @brief Types of data that can be printed (used with `getDataToPrint()` method)
    enum class PrintData{
        WALLET_TXs,
        BLOCKS,
        BLOCK_TXs,
        ACCOUNTS,
        CHAIN,
        CONNECTIONS
    };

    /// @brief Get data for printing
    /// @param ss [out] string stream where to put the data
    /// @param whatData type of data to get
    /// @param id if there are some specific data then this will be identifier of that data 
    void getDataToPrint(std::stringstream &ss, PrintData whatData, std::string id = "");

};


} // namespace PQB

/* END OF FILE */