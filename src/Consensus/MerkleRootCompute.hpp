/**
 * @file MerkleRootCompute.hpp
 * @author Michal Ľaš
 * @brief Functions for computing Merkle Root hash
 * @date 2024-02-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once


#include <vector>

#include "Blob.hpp"
#include "Block.hpp"


namespace PQB{

/**
 * @brief Compute Merkle Root Hash for given vector
 * 
 * @param leafsHashes pointers to vector with SHA-512 hashes
 * @return byte64_t SHA-512 Merkle Root Hash
 * 
 * @note This function should not be used for directly calculating block's Merkle tree root hash!
 * This is because a vector can have duplicit elements in it, which can lead to invalid calculation
 * of block's Merke tree root hash. Use rather ComputeBlocksMerkleRoot(PQB::Block &block) function.
 * 
 * @warning This algorithm is not optimized!
 */
byte64_t ComputeMerkleRoot(std::vector<byte64_t> leafsHashes);


/**
 * @brief Compute Merkle Tree Root of block's transactions
 * 
 * @param block 
 * @return byte64_t SHA-512 Merkle Root Hash
 */
byte64_t ComputeBlocksMerkleRoot(PQB::Block &block);

} // PQB namespace


/* END OF FILE */