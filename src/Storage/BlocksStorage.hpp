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
     * @brief Get the block data from database
     * 
     * @param blockHeight block height (number representiong position of the block in the blockchain)
     * @return Block* pointer to block's data
     */
    Block* getBlock(size_t blockHeight);

    /**
     * @brief Put block into database
     * 
     * @param block block data that will be inserted in database
     */
    void saveBlock(Block& block);

private:
    leveldb::Options databaseOptions;
    leveldb::DB* db; ///< Instance of RocksDB database
};


} // namespace PQB

/* END OF FILE */