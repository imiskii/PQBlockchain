/**
 * @file Transaction.cpp
 * @author Michal Ľaš
 * @brief 
 * @date 2024-02-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Transaction.hpp"
#include "HashManager.hpp"


namespace PQB{

    void Transaction::setHash(){
        TransactionData data = getTransactionData();
        HashMan::SHA512_hash(&IDHash, (PQB::byte*) &data, sizeof(data));
    }

} // namespace PQB


/* END OF FILE */