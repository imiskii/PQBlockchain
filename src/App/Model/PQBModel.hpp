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
#include "PQBconstatnts.hpp"
#include "Wallet.hpp"
#include "BlocksStorage.hpp"
#include "AccountStorage.hpp"
#include "MessageManagement.hpp"
#include "Consensus.hpp"

namespace PQB{

class PQBModel{
private:
    Wallet *wallet;
    BlocksStorage *blockS;
    AccountStorage *accS;
    ConnectionManager *connMng;
    MessageProcessor *msgPrc;
    Consensus *consensus;

    /// @brief Trys to open all configuration files and databases needed for PQB. If operation success return true, else return false
    bool openConfigurationAndDatabase();

public:

    PQBModel(std::string &config_file_path);
    ~PQBModel();

    /// @brief Trys to initialize ConnectionManager, MessageProcessor and Consensus()
    /// @return true if operation is successful false if operation fails
    bool initializeManagers(NodeType node_type);

    /**
     * @brief Create new Transaction broadcast it to peers
     * 
     * @param receiver wallet address of the receiver
     * @param amount amount of resources to transfer
     * @return std::string status of this operation
     */
    std::string createTransaction(std::string &receiver, PQB::cash amount);

};


} // namespace PQB

/* END OF FILE */