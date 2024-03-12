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

#include "PQBExceptions.hpp"
#include "Serialize.hpp"
#include "Signer.hpp"
#include "PQBtypedefs.hpp"
#include "Blob.hpp"

namespace PQB{


class TransactionData
{
public:
    uint32_t versionNumber;         ///< version of the transaction
    uint32_t sequenceNumber;        ///< sequence number of the transaction 
    PQB::cash cashAmount;           ///< amount of transfered resources
    PQB::timestamp timestamp;       ///< creation timestamp 
    byte64_t senderWalletAddress;   ///< address of sender
    byte64_t receiverWalletAddress; ///< address of receiver

    TransactionData(){
        setNull();
    }

    /// @brief Set attributes of TransactionData to zeros
    void setNull(){
        versionNumber = 0;
        sequenceNumber = 0;
        timestamp = 0;
        senderWalletAddress.SetNull();
        receiverWalletAddress.SetNull();
        cashAmount = 0;
    }

    /// @brief Get size of the TransactionData in bytes
    size_t getSize() const;

    /// @brief Serialize TransactionData
    /// @param buffer buffer for serialization
    /// @param offset offset to the buffer
    void serialize(byteBuffer &buffer, size_t &offset) const;

    /// @brief Deserialize TransactionData
    /// @param buffer buffer with serialized data
    /// @param offset offset to the buffer
    void deserialize(const byteBuffer &buffer, size_t &offset);
};


class Transaction : public TransactionData
{
public:
    byte64_t IDHash;        ///< Hash of the transaction
    byteBuffer signature;   ///< Digital signature of the transaction
    uint32_t signatureSize; ///< Size of the digital signature in bytes

    Transaction(){
        setNull();
    }

    /// @brief Set attributes of Transaction to zeros
    void setNull(){
        TransactionData::setNull();
        IDHash.SetNull();
        signatureSize = 0;
    }

    /// @brief Return class with TransactionData
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

    /// @brief Calculates has of the transaction (stored in IDHash attribute)
    void setHash();

    /// @brief Sign the transaction
    void sign(byteBuffer &privateKey);

    /// @brief Verify transaction signature
    bool verify(byteBuffer &publicKey);

    /// @brief Get size of the Transaction in bytes
    size_t getSize() const;

    /// @brief Serialize Transaction
    /// @param buffer buffer for serialization
    /// @param offset offset to the buffer
    void serialize(byteBuffer &buffer, size_t &offset) const;

    /// @brief Deserialize Transaction
    /// @param buffer buffer with serialized data
    /// @param offset offset to the buffer
    void deserialize(const byteBuffer &buffer, size_t &offset);
};


typedef std::shared_ptr<Transaction> TransactionPtr; ///< Shared pointer to transaction

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