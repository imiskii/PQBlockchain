/**
 * @file Block.cpp
 * @author Michal Ľaš
 * @brief Implementation BlockHeader, BlockBody and Block
 * @date 2024-02-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Block.hpp"
#include "HashManager.hpp"


namespace PQB{

    byte64_t BlockHeader::getBlockHash() const{
        byte64_t blockHash;
        byteBuffer buffer;
        size_t offset = 0;
        buffer.resize(getSize());
        serialize(buffer, offset);
        HashMan::SHA512_hash(&blockHash, buffer.data(), buffer.size());
        return blockHash;
    }

    size_t BlockHeader::getSize(){
        return sizeof(version) +
               sizeof(sequence) +
               sizeof(size) +
               sizeof(transactionsMerkleRootHash) +
               sizeof(previousBlockHash) +
               sizeof(accountBalanceMerkleRootHash);
    }

    void BlockHeader::serialize(byteBuffer &buffer, size_t &offset) const{
        if ((buffer.size() - offset) < getSize())
            throw PQB::Exceptions::Block("Serialization: buffer size is too small for serialization");
        if (transactionsMerkleRootHash.IsNull())
            throw PQB::Exceptions::Block("Serialization: Missing block's transactionsMerkleRootHash");
        if (previousBlockHash.IsNull())
            throw PQB::Exceptions::Block("Serialization: Missing block's previousBlockHash");
        if (accountBalanceMerkleRootHash.IsNull())
            throw PQB::Exceptions::Block("Serialization: Missing block's accountBalanceMerkleRootHash");
        serializeField(buffer, offset, version);
        serializeField(buffer, offset, sequence);
        serializeField(buffer, offset, size);
        serializeField(buffer, offset, transactionsMerkleRootHash);
        serializeField(buffer, offset, previousBlockHash);
        serializeField(buffer, offset, accountBalanceMerkleRootHash);
    }

    void BlockHeader::deserialize(const byteBuffer &buffer, size_t &offset){
        if ((buffer.size() - offset) < getSize())
            throw PQB::Exceptions::Block("Deserialization: buffer size is too small for deserialization");
        deserializeField(buffer, offset, version);
        deserializeField(buffer, offset, sequence);
        deserializeField(buffer, offset, size);
        deserializeField(buffer, offset, transactionsMerkleRootHash);
        deserializeField(buffer, offset, previousBlockHash);
        deserializeField(buffer, offset, accountBalanceMerkleRootHash);
    }

    void BlockBody::addTransaction(TransactionPtr tx){
        txSet.insert(tx);
        transactionCount++;
    }

    size_t BlockBody::getSize() const{
        size_t txSetSize = 0;
        for (const auto &tx : txSet){
            txSetSize += tx->getSize();
        }
        return txSetSize + sizeof(transactionCount);
    }

    void BlockBody::serialize(byteBuffer &buffer, size_t &offset) const{
        if ((buffer.size() - offset) < getSize())
            throw PQB::Exceptions::Block("Serialization: buffer size is too small for serialization");
        serializeField(buffer, offset, transactionCount);
        for (const auto &tx : txSet){
            tx->serialize(buffer, offset);
        }
    }

    void BlockBody::deserialize(const byteBuffer &buffer, size_t &offset){
        if ((buffer.size() - offset) < sizeof(transactionCount))
            throw PQB::Exceptions::Block("Deserialization: buffer size is too small for deserialization");
        deserializeField(buffer, offset, transactionCount);
        for (size_t i = 0; i < transactionCount; i++){
            TransactionPtr tx = std::make_shared<Transaction>();
            tx->deserialize(buffer, offset);
            txSet.insert(tx);
        }
    }

    size_t Block::getSize() const{
        return BlockHeader::getSize() + BlockBody::getSize();
    }

    void Block::serialize(byteBuffer &buffer, size_t &offset) const{
        BlockHeader::serialize(buffer, offset);
        BlockBody::serialize(buffer, offset);
    }

    void Block::deserialize(const byteBuffer &buffer, size_t &offset){
        BlockHeader::deserialize(buffer, offset);
        BlockBody::deserialize(buffer, offset);
    }


} // namespace PQB

/* END OF FILE */