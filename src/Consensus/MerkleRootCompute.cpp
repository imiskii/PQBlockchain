/**
 * @file MerkleRootCompute.cpp
 * @author Michal Ľaš
 * @brief Functions for computing Merkle Root hash
 * @date 2024-02-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "MerkleRootCompute.hpp"
#include "HashManager.hpp"


namespace PQB{


byte64_t ComputeMerkleRoot(std::vector<byte64_t> leafsHashes){
    CryptoPP::SHA512 hash;
    while (leafsHashes.size() > 1)
    {
        if(leafsHashes.size() & 1){
            leafsHashes.push_back(leafsHashes.back());
        }
        for(size_t i = 0, j = 0; i < leafsHashes.size(); i += 2, j++){
            HashMan::SHA512_hash(&(leafsHashes[j]), leafsHashes[i], leafsHashes[i+1]);
        }
        leafsHashes.resize(leafsHashes.size() / 2);
    }
    return leafsHashes[0];
}


byte64_t ComputeBlocksMerkleRoot(PQB::Block &block){
    std::vector<byte64_t> txHashes;
    if(block.txSet.size() & 1){
        txHashes.reserve(block.txSet.size()+1);
    }else{
        txHashes.reserve(block.txSet.size());
    }
    
    for(const auto &tx : block.txSet){
        txHashes.push_back(tx->IDHash);
    }
    return ComputeMerkleRoot(std::move(txHashes));
}


} // PQB namespace



/* END OF FILE */