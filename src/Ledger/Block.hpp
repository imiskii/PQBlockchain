/**
 * @file Block.hpp
 * @author Michal Ľaš
 * @brief Implementation BlockHeader, BlockBody and Block
 * @date 2024-02-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once


#include <set>
#include <vector>
#include <memory>

#include "PQBtypedefs.hpp"
#include "Transaction.hpp"
#include "Serialize.hpp"
#include "Blob.hpp"


namespace PQB{


class BlockHeader
{
public:
    uint32_t version;                       ///< Version of the block
    uint32_t sequence;                      ///< Sequence number (block height)
    uint32_t size;                          ///< Block size in bytes
    byte64_t transactionsMerkleRootHash;    ///< Merkle root hash of the block's transactions
    byte64_t previousBlockHash;             ///< Hash of the previous block
    byte64_t accountBalanceMerkleRootHash;  ///< Hash of the account balances

    BlockHeader(){
        setNull();
    }

    /// @brief Set attributes of the BlockHeader to zeros
    void setNull(){
        version = 0;
        sequence = 0;
        transactionsMerkleRootHash.SetNull();
        previousBlockHash.SetNull();
        accountBalanceMerkleRootHash.SetNull();
        size = 0;
    }

    /// @brief Get the Hash of block's header
    /// @return SHA-512 hash of the block
    byte64_t getBlockHash() const;

    /// @brief Get size of the BlockHeader in bytes
    static size_t getSize();

    /// @brief Serialize BlockHeader
    /// @param buffer buffer for serialization
    /// @param offset offset to the buffer
    void serialize(byteBuffer &buffer, size_t &offset) const;

    /// @brief Deserialize BlockHeader
    /// @param buffer buffer with serialized data
    /// @param offset offset to the buffer
    void deserialize(const byteBuffer &buffer, size_t &offset);
};


class BlockBody
{
public:
    uint32_t transactionCount;
    TransactionSet txSet; ///< Transaction set

    BlockBody(){
        setNull();
    }

    /// @brief Set attributes of the BlockBody to zeros and clear all elements of txSet
    void setNull(){
        transactionCount = 0;
        txSet.clear();
    }

    /// @brief Add a transaction to txSet
    void addTransaction(TransactionPtr tx);

    /// @brief Get size of the BlockBody in bytes
    size_t getSize() const;

    /// @brief Serialize BlockBody
    /// @param buffer buffer for serialization
    /// @param offset offset to the buffer
    /// @exception if buffer has not enough size for the block serialization or if important fields are not specified (such as transactionsMerkleRootHash, previousBlockHash, and accountBalanceMerkleRootHash)
    void serialize(byteBuffer &buffer, size_t &offset) const;

    /// @brief Deserialize BlockBody
    /// @param buffer buffer with serialized data
    /// @param offset offset to the buffer
    /// @exception if buffer has not enough size for the block deserialization
    void deserialize(const byteBuffer &buffer, size_t &offset);

};


class Block : public BlockHeader, public BlockBody
{
public:
    Block(){ 
        setNull(); 
    }

    Block(const BlockHeader &header) : BlockHeader(header){
        BlockBody::setNull();
    }

    /// @brief Set attributes of the Block to zeros and clear all elements of txSet
    void setNull(){
        BlockHeader::setNull();
        BlockBody::setNull();
    }

    /// @brief Get header class of the Block
    BlockHeader getBlockHeader() const{
        BlockHeader hdr;
        hdr.version = version;
        hdr.sequence = sequence;
        hdr.transactionsMerkleRootHash = transactionsMerkleRootHash;
        hdr.previousBlockHash = previousBlockHash;
        hdr.accountBalanceMerkleRootHash = accountBalanceMerkleRootHash;
        hdr.size = size;
        return hdr;
    }

    /// @brief Get size of the Block in bytes
    size_t getSize() const;

    /// @brief Serialize Block
    /// @param buffer buffer for serialization
    /// @param offset offset to the buffer
    /// @exception if buffer has not enough size for the block serialization or importnat fields in BlockHeader are not specified (such as transactionsMerkleRootHash, previousBlockHash, and accountBalanceMerkleRootHash)
    void serialize(byteBuffer &buffer, size_t &offset) const;

    /// @brief Deserialize Block
    /// @param buffer buffer with serialized data
    /// @param offset offset to the buffer
    /// @exception if buffer has not enough size for the block deserialization
    void deserialize(const byteBuffer &buffer, size_t &offset);
};



typedef std::shared_ptr<Block> BlockPtr;
typedef std::shared_ptr<BlockHeader> BlockHeaderPtr;


} // namespace PQB


/* END OF FILE */