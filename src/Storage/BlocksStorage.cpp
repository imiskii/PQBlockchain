/**
 * @file BlocksStorage.cpp
 * @author Michal Ľaš
 * @brief LevelDB databese for storing blockchain blocks (metadata + transactions)
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "BlocksStorage.hpp"
#include "PQBExceptions.hpp"
#include "PQBconstatnts.hpp"

namespace PQB{


BlocksStorage::BlocksStorage(){
    db = nullptr;
    databaseOptions.create_if_missing = true;
    databaseOptions.block_size = MAX_BLOCK_SIZE;
    databaseOptions.block_cache = leveldb::NewLRUCache(2 * MAX_BLOCK_SIZE); /// 2MB, just for optimalization. This database wont be working with more than one block at once.
}

BlocksStorage::~BlocksStorage(){
    if(db != nullptr){
        delete db;
        delete databaseOptions.block_cache;
    }
        
}

void BlocksStorage::openDatabase(){
    leveldb::Status status = leveldb::DB::Open(databaseOptions, PQB::BLOCKS_DATABASE_PATH, &db);
    if(!status.ok()){
        throw PQB::Exceptions::Storage(status.ToString());
    }
}

Block *BlocksStorage::getBlock(const byte64_t &blockHash){
    std::string readValue;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), leveldb::Slice((char*)blockHash.data(), blockHash.size()), &readValue);
    if (status.IsNotFound()){
        return nullptr;
    } else if (!status.ok()){
        /// @todo make log -> status.ToString()
        return nullptr;
    }
    byteBuffer buffer(readValue.begin(), readValue.end());
    Block *newBlock = new Block();
    size_t offset = 0;
    newBlock->deserialize(buffer, offset);
    return newBlock;
}

bool BlocksStorage::getRawBlock(const byte64_t &blockHash, byteBuffer &buffer){
    std::string readValue;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), leveldb::Slice((char*)blockHash.data(), blockHash.size()), &readValue);
    if (status.IsNotFound()){
        return false;
    } else if (!status.ok()){
        /// @todo make log -> status.ToString()
        return false;
    }
    buffer.resize(readValue.size());
    std::memcpy(buffer.data(), readValue.data(), readValue.size());
    return false;
}

bool BlocksStorage::setBlock(const Block &block)
{
    byteBuffer buffer;
    size_t offset = 0;
    block.serialize(buffer, offset);
    byte64_t blockHash = block.getBlockHash();
    if (setBlock(blockHash, buffer))
        return true;
    return false;
}

bool BlocksStorage::setBlock(const byte64_t &blockHash, const byteBuffer &buffer){
    leveldb::Slice value((char*) buffer.data(), buffer.size());
    leveldb::Status status = db->Put(leveldb::WriteOptions(), leveldb::Slice((char*)blockHash.data(), blockHash.size()), value);
    if (!status.ok())
        return false;
    return true;

}

} // namespace PQB

/* END OF FILE */