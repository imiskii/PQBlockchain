/**
 * @file Consensus.cpp
 * @author Michal Ľaš
 * @brief 
 * @date 2024-03-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Consensus.hpp"

namespace PQB{

    bool Consensus::isTransactionInPool(const byte64_t tx_id) const{
        if (txPool.find(tx_id) != txPool.end()){
            return true;
        }
        return false;
    }

    TransactionPtr Consensus::getTransactionFromPool(const byte64_t tx_id) const{
        const auto &it = txPool.find(tx_id);
        if (it == txPool.end()){
            return it->second;
        }
        return nullptr;
    }

    void Consensus::addTransactionToPool(TransactionPtr tx){
        const auto &it = txPool.find(tx->IDHash);
        if (it == txPool.end()){
            txPool.emplace(tx->IDHash, tx);
        }
    }

    void Consensus::removeTransactionFromPool(const byte64_t tx_id){
        txPool.erase(tx_id);
    }

} // namespace PQB

/* END OF FILE */