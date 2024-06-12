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

    bool Transaction::checkTransactionStructure() const{
        if (versionNumber == 0 ||
            sequenceNumber == 0 ||
            senderWalletAddress.IsNull() ||
            receiverWalletAddress.IsNull() ||
            IDHash.IsNull() ||
            signature.size() == 0 ||
            signatureSize == 0){
                return false;
            }
        return true;
    }

    void Transaction::setHash(){
        byteBuffer buffer;
        size_t offset = 0;
        buffer.resize(TransactionData::getSize());
        TransactionData::serialize(buffer, offset);
        HashMan::SHA512_hash(&IDHash, buffer.data(), buffer.size());
    }

    void Transaction::sign(byteBuffer &privateKey){
        if (IDHash.IsNull())
            throw PQB::Exceptions::Transaction("Sign: transaction does not have an ID!");
        SignAlgorithmPtr s = Signer::GetInstance();
        signatureSize = s->sign(signature, IDHash.data(), IDHash.size(), privateKey);

    }

    bool Transaction::verify(byteBuffer &publicKey){
        SignAlgorithmPtr s = Signer::GetInstance();
        if (s->verify(signature, IDHash.data(), IDHash.size(), publicKey)){
            return true;
        }
        return false;
    }

    size_t Transaction::getSize() const{
        if (signatureSize == 0){
            throw PQB::Exceptions::Transaction("getSize: transaction is not sign! Signature size is unknow!");
        }
        return TransactionData::getSize() + sizeof(IDHash) + sizeof(signatureSize) + signatureSize;
    }

    void Transaction::serialize(byteBuffer &buffer, size_t &offset) const{
        if (IDHash.IsNull())
            throw PQB::Exceptions::Transaction("Serialization: transaction does not have an ID!");
        if ((buffer.size() - offset) < getSize())
            throw PQB::Exceptions::Transaction("Serialization: serialization buffer has not enough size to serialize the transaction");
        TransactionData::serialize(buffer, offset);
        serializeField(buffer, offset, IDHash);
        serializeField(buffer, offset, signatureSize);
        std::memcpy(buffer.data() + offset, signature.data(), signatureSize);
        offset += signatureSize;
    }

    void Transaction::deserialize(const byteBuffer &buffer, size_t &offset){
        if ((buffer.size() - offset) < (TransactionData::getSize() + sizeof(IDHash) + sizeof(signatureSize)))
            throw PQB::Exceptions::Transaction("Deserialization: buffer has not enough size to deserialize the transaction");
        TransactionData::deserialize(buffer, offset);
        deserializeField(buffer, offset, IDHash);
        deserializeField(buffer, offset, signatureSize);
        if ((buffer.size() - offset) < signatureSize)
            throw PQB::Exceptions::Transaction("Deserialization: buffer has not enough size to deserialize the transaction");
        signature.resize(signatureSize);
        std::memcpy(signature.data(), buffer.data() + offset, signatureSize);
        offset += signatureSize;
    }

    size_t TransactionData::getSize() const{
        return sizeof(versionNumber) + 
               sizeof(sequenceNumber) +
               sizeof(cashAmount) +
               sizeof(timestamp) +
               sizeof(senderWalletAddress) +
               sizeof(receiverWalletAddress);
    }

    void TransactionData::serialize(byteBuffer &buffer, size_t &offset) const{
        if (senderWalletAddress.IsNull())
            throw PQB::Exceptions::Transaction("Serialization: transaction does not have a senderWalletAddress!");
        if (receiverWalletAddress.IsNull()){
            throw PQB::Exceptions::Transaction("Serialization: transaction does not have a receiverWalletAddress!");
        }
        serializeField(buffer, offset, versionNumber);
        serializeField(buffer, offset, sequenceNumber);
        serializeField(buffer, offset, cashAmount);
        serializeField(buffer, offset, timestamp);
        serializeField(buffer, offset, senderWalletAddress);
        serializeField(buffer, offset, receiverWalletAddress);
    }

    void TransactionData::deserialize(const byteBuffer &buffer, size_t &offset){
        deserializeField(buffer, offset, versionNumber);
        deserializeField(buffer, offset, sequenceNumber);
        deserializeField(buffer, offset, cashAmount);
        deserializeField(buffer, offset, timestamp);
        deserializeField(buffer, offset, senderWalletAddress);
        deserializeField(buffer, offset, receiverWalletAddress);
    }

} // namespace PQB

/* END OF FILE */