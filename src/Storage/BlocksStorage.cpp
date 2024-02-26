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
}

BlocksStorage::~BlocksStorage(){
    if(db != nullptr)
        delete db;
}

void BlocksStorage::openDatabase(){
    leveldb::Status status = leveldb::DB::Open(databaseOptions, PQB::BLOCKS_DATABASE_PATH, &db);
    if(!status.ok()){
        throw PQB::Exceptions::Storage(status.ToString());
    }
}

Block *BlocksStorage::getBlock(size_t blockHeight){
    return nullptr;
}

void BlocksStorage::saveBlock(Block &block){

}


} // namespace PQB

/* END OF FILE */