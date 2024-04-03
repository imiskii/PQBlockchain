/**
 * @file Chain.hpp
 * @author Michal Ľaš
 * @brief Implementation of tree structure for storing blocks
 * @date 2024-03-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <utility>
#include "Blob.hpp"
#include "Block.hpp"


namespace PQB{


/**
 * @brief Representation of a block in chain tree
 * 
 */
struct BlockNode{

    using ID = byte64_t;

    // id is hash of the block, but with accountMerkleRoothHash set to Null. It is because this root hash is unknow until full validation
    // and execution of transactions. The real hash of the block can be optained from block->getHash() in case that this block was validated.
    ID id;  ///< block hash
    BlockHeaderPtr block;
    BlockNode *parent;
    BlockNode *validChild; ///< nullptr until a child is not validated
    std::vector<BlockNode*> childs;

    std::uint32_t tipSupport;

    inline bool operator==(const BlockNode &other){
        return other.id == this->id;
    }
};


/// @brief Tree with block headers
class Chain{

    using ChainIn = std::map<BlockNode::ID, BlockNode>;
    using Validations = std::map<std::string, BlockNode*>; ///< issuers mapped to thier last proposal

    ChainIn ch;
    BlockNode validBlock; ///< last valid block
    std::uint32_t UNLcount; ///< number of nodes on the UNL
    std::string thisNodeId; ///< local validator ID
    Validations validations;

public:
    
    Chain(std::uint32_t sizeOfUNL, std::string localNodeId) : validBlock(getGenesisBlockNode()), UNLcount(sizeOfUNL), thisNodeId(localNodeId) {
        ch.emplace(validBlock.id, validBlock);
        validations.clear();
    }
    
    /**
     * @brief Insert new block into the chain
     * 
     * @param issuer Issuer of the block
     * @param block Block header to put into the chain
     */
    void insert(std::string issuer, BlockHeaderPtr block);

    /// @brief If conditions are met update valid block to block with given `block_id`. 
    /// If there is not such a block do nothing and return false.
    /// This method should be called after insert method to check if current working block reach consensus
    /// @return true if block reach consensus and becomes new valid block, false if not
    bool updateValidBlock(byte64_t &block_id);

    /// @brief Once block was validated, its transactions executed, and the new account hash calculated
    /// the new account hash should be placed to this block with this method
    /// @param accHash new account hash
    void assignAccountHashToValidBlock(byte64_t &accHash);

    /// @brief Get ID and header of Block on which majority of UNL nodes are woking on
    std::pair<byte64_t&, BlockHeaderPtr> getPreferredBlock();

    static BlockPtr getGenesisBlock(){
        BlockPtr genesis = std::make_shared<Block>();
        genesis->getSize();
        return genesis;
    }

private:

    /// @brief Get preferred block of given block `n` with its ID
    std::pair<byte64_t&, BlockHeaderPtr> getPreferredBlock(BlockNode *n);

    /// @brief return hardcoded genesis block
    static BlockNode getGenesisBlockNode(){
        BlockHeaderPtr genesis = getGenesisBlock();

        BlockNode genesisRef;
        genesisRef.id = genesis->getBlockHash();
        genesisRef.parent = nullptr;
        genesisRef.validChild = nullptr;
        genesisRef.childs.clear();
        genesisRef.tipSupport = 0;
        return genesisRef;
    }

    /// @brief Check if block `n` does reach quorum for becoming new validBlock
    bool checkQuorum(BlockNode &n);

    /// @brief Get sum of the tip support of `n` and all its descendants in tree
    /// @param n structure referencing to a block
    /// @return support of the `n` block
    std::uint32_t getSupport(BlockNode &n);

    /// @brief Get sum of validators that have not voted yet for any ledger on `n` sequence
    /// @param n structure referencing to a block
    /// @return number of uncommitted validators at `n` BlockNode sequence
    std::uint32_t getUncommitted(BlockNode &n);

};



} // namespace PQB

/* END OF FILE */