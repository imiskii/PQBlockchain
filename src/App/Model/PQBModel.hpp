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

#include "Wallet.hpp"
#include "BlocksStorage.hpp"
#include "AccountStorage.hpp"
#include "Connection.hpp"
#include "MessageManagement.hpp"


namespace PQB{

class PQBModel{
private:
    Wallet *wallet;
    BlocksStorage *blockS;
    AccountStorage *accS;
    ConnectionManager *connMng;
    MessageProcessor *msgPrc;

public:

    PQBModel();
    ~PQBModel();


};


} // namespace PQB

/* END OF FILE */