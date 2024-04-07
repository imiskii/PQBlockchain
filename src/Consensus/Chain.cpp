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
#include "Log.hpp"

namespace PQB{
    
    
    void Chain::insert(std::string issuer, BlockHeaderPtr block, bool isLocal){
        auto lastRef = validations.find(issuer);
        if ((lastRef != validations.end()) && !isLocal){ // if issuer already made a validation update tip-support
            if (lastRef->second != nullptr){
                lastRef->second->tipSupport--;
            }
        }
        BlockNode::ID blockId = block->getBlockHash();
        auto exNode = ch.find(blockId);
        if (exNode != ch.end()){ // The same block is already in tree, just update its tip-support
            if (!isLocal){
                exNode->second.tipSupport++;
            }
            if (lastRef != validations.end())
                lastRef->second = &exNode->second;
            else
                validations.emplace(issuer, &exNode->second);
            PQB_LOG_TRACE("CHAIN", "Node {} updated its block to {}. New tip-support is {}",  shortStr(issuer), shortStr(blockId.getHex()), exNode->second.tipSupport);
        } else { // Block is not in tree yet, add the new block
            BlockNode newNode;
            newNode.id = blockId;
            newNode.block = block;
            newNode.childs.clear();
            newNode.validChild = nullptr;
            newNode.tipSupport = (isLocal) ? 0 : 1;
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
                    PQB_LOG_TRACE("CHAIN", "Block {} inserted by {} with tip-support: {}", shortStr(blockId.getHex()), shortStr(issuer), newNode.tipSupport);
                } else {
                    PQB_LOG_ERROR("CHAIN", "Block was not inserted into the chain because it already exists");
                }
            } else {
                PQB_LOG_ERROR("CHAIN", "Block was not inserted into the chain because of unknow parent block");
            }
        }
    }

    bool Chain::updateValidBlock(byte64_t &block_id){
        const auto it = ch.find(block_id);
        if (it != ch.end()){
            if (checkQuorum(it->second)){
                validBlock = it->second;
                PQB_LOG_TRACE("CHAIN", "New valid block is: {}", shortStr(validBlock.id.getHex()));
                return true;
            }
        }
        return false;
    }

    void Chain::assignAccountHashToValidBlock(byte64_t &accHash){
        auto it = ch.find(validBlock.id);
        validBlock.block->accountBalanceMerkleRootHash = accHash;
        validBlock.id = validBlock.block->getBlockHash();
        auto [newit, result] = ch.emplace(validBlock.id, validBlock);
        if (result){
            it->second.parent->validChild = &newit->second; // replace pointer to valid block of the parent
            for (auto &peer : validations){
                // if block proposed by peer is new validated block replace pointer to it
                if (peer.second->id == it->second.id)
                    peer.second = &newit->second;
            }
        }
        PQB_LOG_TRACE("CHAIN", "New valid block hash is: {}", shortStr(validBlock.id.getHex()));
    }

    void Chain::putChainDataToStringStream(std::stringstream &ss){
        byte64_t genesisId;
        genesisId.setHex(std::string(GENESIS_BLOCK_HASH));
        const auto it = ch.find(genesisId);
        if (it != ch.end()){
            ss << "Genesis Block" << std::endl;
            ss << " | " << std::endl;
            for (const auto &child : it->second.childs){
                ss << " | ->" << child->id.getHex() << std::endl;
            }
            ss << " | " << std::endl;

            BlockNode *node = it->second.validChild;
            while (node != nullptr){
                ss << " V " << std::endl;
                ss << node->id.getHex() << std::endl;
                ss << " | " << std::endl;
                for (const auto &child : node->childs){
                    ss << " | ->" << child->id.getHex() << std::endl;
                }
                ss << " | " << std::endl;
                node = node->validChild;
            }
            ss << std::endl;
        }
        
    }

    std::pair<byte64_t&, BlockHeaderPtr&> Chain::getPreferredBlock(){
        return getPreferredBlock(&validBlock);
    }

    std::pair<byte64_t&, BlockHeaderPtr&> Chain::getPreferredBlock(BlockNode *n)
    {
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
        if ((n.tipSupport > (0.8 * UNLcount)) && (n.block->sequence > validBlock.block->sequence)){
            PQB_LOG_TRACE("CHAIN", "Quorum for new valid block reached. Chosen block {} with tip-support {}", shortStr(n.id.getHex()), n.tipSupport);
            return true;
        }
        PQB_LOG_TRACE("CHAIN", "Quorum still not reached. Block {} has tip-support {}", shortStr(n.id.getHex()), n.tipSupport);
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
        std::uint32_t cnt = UNLcount;
        for (const auto &it : validations){
            if (it.second != nullptr){
                if (it.second->block->sequence >= n.block->sequence){
                    --cnt;
                }
            }
        }
        return cnt;
    }

} // namespace PQB

/* END OF FILE */