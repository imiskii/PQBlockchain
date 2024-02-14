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

    byte64_t Block::getBlockHash() const{
        byte64_t blockHash;
        BlockHeader blockHdr = getBlockHeader();
        HashMan::SHA512_hash(&blockHash, (PQB::byte*) &blockHdr, sizeof(blockHdr));
        return blockHash;
    }

} // namespace PQB

/* END OF FILE */