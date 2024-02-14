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

#include "PQBtypedefs.hpp"
#include "Transaction.hpp"
#include "Blob.hpp"


namespace PQB{


class BlockHeader
{
public:
    uint32_t version;
    byte64_t merkleRootHash;
    byte64_t previousBlockHash;
    PQB::timestamp timestamp;
    uint32_t size;

    BlockHeader(){
        setNull();
    }

    void setNull(){
        version = 0;
        merkleRootHash.SetNull();
        previousBlockHash.SetNull();
        timestamp = 0;
        size = 0;
    }
};


class BlockBody
{
public:
    std::set<TransactionPtr, TransactionPtrComparator> txSet; ///< Transaction set

    BlockBody(){
        setNull();
    }

    void setNull(){
        txSet.clear();
    }

};


class Block : public BlockHeader, public BlockBody
{
private:
    /* data */
public:
    Block(){ 
        setNull(); 
    }

    void setNull(){
        BlockHeader::setNull();
        BlockBody::setNull();
    }

    BlockHeader getBlockHeader() const{
        BlockHeader hdr;
        hdr.version = version;
        hdr.merkleRootHash = merkleRootHash;
        hdr.previousBlockHash = previousBlockHash;
        hdr.timestamp = timestamp;
        hdr.size = size;
        return hdr;
    }

    /**
     * @brief Get the Hash of block's header
     * 
     * @return byte64_t 
     */
    byte64_t getBlockHash() const;


    void addTransaction(TransactionPtr tx);

};


} // namespace PQB


/* END OF FILE */