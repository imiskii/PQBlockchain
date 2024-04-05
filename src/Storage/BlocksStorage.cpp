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
#include "PQBconstants.hpp"
#include "Log.hpp"

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
    }
    delete databaseOptions.block_cache;
        
}

void BlocksStorage::openDatabase(){
    leveldb::Status status = leveldb::DB::Open(databaseOptions, PQB::BLOCKS_DATABASE_PATH.data(), &db);
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
        PQB_LOG_ERROR("BLOCK STORAGE", "Failed to get block: {}", status.ToString());
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
        PQB_LOG_ERROR("BLOCK STORAGE", "Failed to get block: {}", status.ToString());
        return false;
    }
    buffer.resize(readValue.size());
    std::memcpy(buffer.data(), readValue.data(), readValue.size());
    return false;
}

bool BlocksStorage::setBlock(const Block *block){
    byteBuffer buffer;
    size_t offset = 0;
    block->serialize(buffer, offset);
    byte64_t blockHash = block->getBlockHash();
    if (setBlock(blockHash, buffer))
        return true;
    return false;
}

bool BlocksStorage::setBlock(const byte64_t &blockHash, const byteBuffer &buffer){
    leveldb::Slice value((char*) buffer.data(), buffer.size());
    leveldb::Status status = db->Put(leveldb::WriteOptions(), leveldb::Slice((char*)blockHash.data(), blockHash.size()), value);
    if (!status.ok()){
        PQB_LOG_ERROR("BLOCK STORAGE", "Failed to get block: {}", status.ToString());
        return false;
    }
    PQB_LOG_TRACE("BLOCK STORAGE", "Block {} added to database", shortStr(blockHash.getHex()));
    return true;
}

void BlocksStorage::putBlockHeadersDataToStringStream(std::stringstream &ss){
    leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());
    Block block;
    byteBuffer buffer;
    for (it->SeekToFirst(); it->Valid(); it->Next()){
        size_t offset = 0;
        buffer.resize(it->value().size());
        std::memcpy(buffer.data(), it->value().data(), buffer.size());
        block.deserialize(buffer, offset);
        
        

        ss
        << "Block: " << bytesToHexString((PQB::byte*)it->key().data(), it->key().size()) << std::endl 
        << "Version: " << block.version << std::endl
        << "Seq.: " << block.sequence << std::endl
        << "Size: " << block.size << std::endl
        << "Tx count: " << block.transactionCount << std::endl
        << "Parent: " << block.previousBlockHash.getHex() << std::endl
        << "Tx hash: " << block.transactionsMerkleRootHash.getHex() << std::endl
        << "Acc hash: " << block.accountBalanceMerkleRootHash.getHex() << std::endl 
        << std::endl << "------------------------------" << std::endl;
    }
    delete it;
}

void BlocksStorage::putBlockTxDataToStringStream(std::string &block_id, std::stringstream &ss){
    byte64_t bid;
    bid.setHex(block_id);
    Block *block;
    block = getBlock(bid);

    for (const auto &tx : block->txSet){
        using clock = std::chrono::system_clock;
        std::time_t time = clock::to_time_t(clock::from_time_t(tx->timestamp));
        std::tm* tm = std::localtime(&time);
        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);

        ss
        << "ID: " << tx->IDHash.getHex() << "" << std::endl
        << "Seq.: " << tx->sequenceNumber << std::endl
        << "Timestamp: " << buffer << std::endl
        << "Amount: " << tx->cashAmount << std::endl
        << "Send.: " << tx->senderWalletAddress.getHex() << std::endl
        << "Recv.: " << tx->receiverWalletAddress.getHex() << std::endl
        << std::endl << "------------------------------" << std::endl;
    }

    delete block;
}

} // namespace PQB

/* END OF FILE */