/**
 * @file Transaction.hpp
 * @author Michal Ľaš
 * @brief 
 * @date 2024-02-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once


#include <chrono>
#include <cstring>

#include "PQBtypedefs.hpp"
#include "Blob.hpp"

namespace PQB{


class TransactionData
{
public:
    uint32_t versionNumber;
    uint32_t sequenceNumber;
    byte64_t senderWalletAddress;
    byte64_t receiverWalletAddress;
    PQB::timestamp timestamp;
    PQB::cash cashAmount;

    TransactionData(){
        setNull();
    }

    void setNull(){
        versionNumber = 0;
        sequenceNumber = 0;
        timestamp = 0;
        senderWalletAddress.SetNull();
        receiverWalletAddress.SetNull();
        cashAmount = 0;
    }
};


class Transaction : public TransactionData
{
public:
    byte64_t IDHash;    ///< Hash of the transaction

    Transaction(){
        setNull();
    };

    void setNull(){
        TransactionData::setNull();
        IDHash.SetNull();
    }

    TransactionData getTransactionData() const{
        TransactionData data;
        data.versionNumber = versionNumber;
        data.sequenceNumber = sequenceNumber;
        data.senderWalletAddress = senderWalletAddress;
        data.receiverWalletAddress = receiverWalletAddress;
        data.timestamp = timestamp;
        data.cashAmount = cashAmount;
        return data;
    }

    /**
     * @brief Calculates has of the Transaction (stored in IDHash attribute)
     * 
     */
    void setHash();
};


typedef std::shared_ptr<const Transaction> TransactionPtr; ///< Shared pointer to transaction

/**
 * @brief Comparator for ordering transactions, that are given as pointers to these transactions. Transactions are ordered by transaction ID.
 * 
 */
struct TransactionPtrComparator{
    bool operator()(const TransactionPtr& a, const TransactionPtr& b) const{
        return a->IDHash < b->IDHash;
    }
};


} // namespace PQB


/* END OF FILE */