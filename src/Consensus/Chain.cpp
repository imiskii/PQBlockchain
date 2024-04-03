/**
 * @file Chain.cpp
 * @author Michal Ľaš
 * @brief Implementation of tree structure for storing blocks
 * @date 2024-03-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Chain.hpp"


namespace PQB{
    
    
    void Chain::insert(std::string issuer, BlockHeaderPtr block){
        auto lastRef = validations.find(issuer);
        if (lastRef != validations.end()){ // if issuer already made a validation update tip-support
            if (lastRef->second != nullptr){
                lastRef->second->tipSupport--;
            }
        }
        BlockNode::ID blockId = block->getBlockHash();
        auto exNode = ch.find(blockId);
        if (exNode != ch.end()){ // The same block is already in tree, just update its tip-support
            exNode->second.tipSupport++;
            if (lastRef != validations.end())
                lastRef->second = &exNode->second;
            else
                validations.emplace(issuer, &exNode->second);
        } else { // Block is not in tree yet, add the new block
            BlockNode newNode;
            newNode.id = blockId;
            newNode.block = block;
            newNode.childs.clear();
            newNode.validChild = nullptr;
            newNode.tipSupport = 1;
            auto parentBlockNode = ch.find(block->previousBlockHash);
            if (parentBlockNode != ch.end()){
                newNode.parent = &parentBlockNode->second;
                auto [it, result] = ch.emplace(blockId, newNode);
                if (result){
                    parentBlockNode->second.childs.push_back(&it->second);
                    if (lastRef != validations.end())
                        lastRef->second = &exNode->second;
                    else
                        validations.emplace(issuer, &exNode->second);
                }
            }
        }
    }

    bool Chain::updateValidBlock(byte64_t &block_id){
        const auto it = ch.find(block_id);
        if (checkQuorum(it->second)){
            if (it != ch.end()){
                validBlock.validChild = &it->second;
                validBlock = it->second;
                return true;
            }
        }
        return false;
    }

    void Chain::assignAccountHashToValidBlock(byte64_t &accHash){
        validBlock.block->accountBalanceMerkleRootHash = accHash;
        auto it = ch.find(validBlock.id);
        it->second.block->accountBalanceMerkleRootHash = accHash;
    }

    std::pair<byte64_t&, BlockHeaderPtr> Chain::getPreferredBlock(){
        return getPreferredBlock(&validBlock);
    }

    std::pair<byte64_t&, BlockHeaderPtr> Chain::getPreferredBlock(BlockNode *n){
        if (n->childs.empty()){
            return {n->id, n->block};
        } else {
            // Get child with greatest support
            BlockNode *bestChild = n->childs.at(0);
            std::uint32_t bestSupport = getSupport(*bestChild);
            std::uint32_t secondBestSupport = 0;
            for (const auto child : n->childs){
                std::uint32_t currSupport = getSupport(*child);
                if (bestSupport < currSupport){
                    secondBestSupport = bestSupport;
                    bestSupport = currSupport;
                    bestChild = child;
                }
            }
            std::uint32_t bestUncommitted = getUncommitted(*bestChild); 
            if (bestUncommitted >= bestSupport){
                return {n->id, n->block};
            } else if ((secondBestSupport + bestUncommitted) < bestSupport){
                return getPreferredBlock(bestChild);
            } else {
                return {n->id, n->block};
            }
        }
    }

    bool Chain::checkQuorum(BlockNode &n){
        // Quorum is 80% of all validators and candidate valid block has to have greater sequence number than current validBlock
        size_t valCount = n.tipSupport;
        // Vote of this node does not count to quorum, just votes from UNL nodes.
        // So if this node vote is the same as checked block decrease tipSupport
        const auto &it = validations.find(thisNodeId);
        if (it != validations.end()){
            if (it->second->id == n.id)
                --valCount;
        }

        if ((valCount > (0.8 * UNLcount)) && (n.block->sequence > validBlock.block->sequence)){
            return true;
        }
        return false;
    }

    std::uint32_t Chain::getSupport(BlockNode &n){
        std::uint32_t cnt = n.tipSupport;
        for (const auto child : n.childs){
            cnt += child->tipSupport;
        }
        return cnt;
    }

    std::uint32_t Chain::getUncommitted(BlockNode &n){
        std::uint32_t cnt = 0;
        for (const auto &it : validations){
            if (it.second != nullptr){
                if (it.second->block->sequence < n.block->sequence){
                    cnt++;
                }
            } else {
                cnt++;
            }
        }
        return cnt;
    }

} // namespace PQB

/* END OF FILE */