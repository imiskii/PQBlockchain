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
               sizeof(size) +
               sizeof(timestamp) +
               sizeof(transactionsMerkleRootHash) +
               sizeof(previousBlockHash) +
               sizeof(accountBalanceMerkleRootHash);
    }

    void BlockHeader::serialize(byteBuffer &buffer, size_t &offset) const{
        if (transactionsMerkleRootHash.IsNull())
            throw PQB::Exceptions::Block("Serialization: Missing block's transactionsMerkleRootHash");
        if (previousBlockHash.IsNull())
            throw PQB::Exceptions::Block("Serialization: Missing block's previousBlockHash");
        if (accountBalanceMerkleRootHash.IsNull())
            throw PQB::Exceptions::Block("Serialization: Missing block's accountBalanceMerkleRootHash");
        serializeField(buffer, offset, version);
        serializeField(buffer, offset, size);
        serializeField(buffer, offset, timestamp);
        serializeField(buffer, offset, transactionsMerkleRootHash);
        serializeField(buffer, offset, previousBlockHash);
        serializeField(buffer, offset, accountBalanceMerkleRootHash);
    }

    void BlockHeader::deserialize(const byteBuffer &buffer, size_t &offset){
        deserializeField(buffer, offset, version);
        deserializeField(buffer, offset, size);
        deserializeField(buffer, offset, timestamp);
        deserializeField(buffer, offset, transactionsMerkleRootHash);
        deserializeField(buffer, offset, previousBlockHash);
        deserializeField(buffer, offset, accountBalanceMerkleRootHash);
    }

    void BlockBody::addTransaction(TransactionPtr tx){
        txSet.insert(tx);
    }

    size_t BlockBody::getSize() const{
        size_t txSetSize = 0;
        for (const auto &tx : txSet){
            txSetSize += tx->getSize();
        }
        return txSetSize + sizeof(transactionCount);
    }

    void BlockBody::serialize(byteBuffer &buffer, size_t &offset) const{
        serializeField(buffer, offset, transactionCount);
        for (const auto &tx : txSet){
            tx->serialize(buffer, offset);
        }
    }

    void BlockBody::deserialize(const byteBuffer &buffer, size_t &offset){
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

    void Block::serialize(byteBuffer &buffer) const{
        size_t offset = 0;
        BlockHeader::serialize(buffer, offset);
        BlockBody::serialize(buffer, offset);
    }

    void Block::deserialize(const byteBuffer &buffer){
        size_t offset = 0;
        BlockHeader::deserialize(buffer, offset);
        BlockBody::deserialize(buffer, offset);
    }


} // namespace PQB

/* END OF FILE */