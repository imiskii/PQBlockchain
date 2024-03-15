/**
 * @file BlocksStorage.hpp
 * @author Michal Ľaš
 * @brief LevelDB databese for storing blockchain blocks (metadata + transactions)
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include "leveldb/db.h"
#include "leveldb/slice.h"
#include "leveldb/cache.h"
#include "Blob.hpp"
#include "Block.hpp"
#include "Transaction.hpp"


namespace PQB{


class BlocksStorage{
public:

    BlocksStorage();
    ~BlocksStorage();

    /**
     * @brief Open LevelDB database
     * @exception If database fails to open
     * 
     */
    void openDatabase();

    /**
     * @brief Query a block data from the database
     * 
     * @param blockHash Identifier of the block to query
     * @return Block* pointer to block's data or nullptr if a block with given hash was not found or database Get error occure
     */
    Block* getBlock(byte64_t &blockHash);

    /**
     * @brief Put block into database
     * 
     * @param block block data that will be inserted into the database
     * @return true if operation was successful
     * @return false if operation fails
     */
    bool setBlock(Block &block);

    /**
     * @brief Put block into database
     * 
     * @param blockHash Identifier of the block
     * @param buffer serialized block data that will be inserted into the database
     * @return true if operation was successful
     * @return false if operation fails
     */
    bool setBlock(byte64_t &blockHash, byteBuffer &buffer);

private:
    leveldb::Options databaseOptions;
    leveldb::DB* db; ///< Instance of LevelDB database
};


} // namespace PQB

/* END OF FILE */