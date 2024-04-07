/**
 * @file Consensus.cpp
 * @author Michal Ľaš
 * @brief Implementation of the Ripple XRP consensus mechanism
 * @date 2024-03-17
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Consensus.hpp"
#include "MessageManagement.hpp"
#include "Log.hpp"

namespace PQB{

    ConsensusWrapper::ConsensusWrapper(Chain *chain, BlocksStorage *blockS, AccountStorage *accS, Wallet *wallet) 
    : chain_(chain), blockS_(blockS), accS_(accS), wallet_(wallet){
        txPool_.clear();
        consensus_ = new Consensus(this);
        consensusRun_ = true;
        consensusThread_ = std::jthread(&ConsensusWrapper::consensusThread, this);
    }

    ConsensusWrapper::~ConsensusWrapper(){
        consensusRun_ = false;
        consensusCondition_.notify_one();
        if (consensusThread_.joinable()){
            consensusThread_.join();
        }
        delete consensus_;
    }

    void ConsensusWrapper::consensusThread(){
        std::chrono::steady_clock::time_point lastTimeEntryPoint = std::chrono::steady_clock::now();
        while (consensusRun_)
        {
            std::unique_lock<std::mutex> lock(consensusMutex_);
            consensusCondition_.wait(lock, [this]{ return (!txPool_.empty() || !consensusRun_); });
            if (!consensusRun_){
                lock.unlock();
                continue;
            }
            // If there were not any transaction for same time reset round with beginConsensus
            if ((std::chrono::steady_clock::now() - lastTimeEntryPoint) > std::chrono::seconds(3)){
                consensus_->beginConsensus();
            }

            consensus_->timerEntry();
            lock.unlock();

            lastTimeEntryPoint = std::chrono::steady_clock::now();
            // Sleep for one second, then activate consensus again
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
    }

    bool ConsensusWrapper::isTransactionInPool(const byte64_t tx_id){
        std::lock_guard<std::mutex> lock(consensusMutex_);
        if (txPool_.find(tx_id) != txPool_.end()){
            return true;
        }
        return false;
    }

    TransactionPtr ConsensusWrapper::getTransactionFromPool(const byte64_t tx_id){
        std::lock_guard<std::mutex> lock(consensusMutex_);
        const auto &it = txPool_.find(tx_id);
        if (it == txPool_.end()){
            return it->second;
        }
        return nullptr;
    }

    void ConsensusWrapper::addTransactionToPool(TransactionPtr tx){
        std::lock_guard<std::mutex> lock(consensusMutex_);
        const auto it = txPool_.find(tx->IDHash);
        if (it == txPool_.end()){
            txPool_.emplace(tx->IDHash, tx);
        }
        consensusCondition_.notify_one();
    }

    void ConsensusWrapper::removeTransactionFromPool(const byte64_t tx_id){
        std::lock_guard<std::mutex> lock(consensusMutex_);
        txPool_.erase(tx_id);
    }

    void ConsensusWrapper::notifyBlockProposal(BlockProposalPtr &prop){
        std::lock_guard<std::mutex> lock(consensusMutex_);
        consensus_->peerProposal(prop);
    }

    void ConsensusWrapper::notifyTxSetProposal(TxSetProposalPtr &prop){
        std::lock_guard<std::mutex> lock(consensusMutex_);
        consensus_->gotTxSet(prop);
    }

    std::pair<byte64_t&, BlockHeaderPtr&> ConsensusWrapper::getPreferred(){
        return chain_->getPreferredBlock();
    }

    void ConsensusWrapper::getCurrentTransactionSet(TransactionSet &set){
        for (const auto &it : txPool_){
            set.insert(it.second);
        }
    }

    size_t ConsensusWrapper::getUNLcount(){
        return wallet_->getUNL().size();
    }

    void ConsensusWrapper::share(TxSetProposal &prop){
        prop.issuer = wallet_->getWalletID();
        prop.sign(wallet_->getSecretKey());
        TxSetProposalMessage *msg = new TxSetProposalMessage(prop.getSize());
        msg->serialize(&prop);
        ConnectionManager::MessageRequest_t req = {.type=ConnectionManager::MessageRequestType::BROADCAST, .connectionID=0, .peerID="", .message=msg};
        connMng_->addMessageRequest(req);
    }

    void ConsensusWrapper::share(BlockProposal &prop){
        prop.issuer = wallet_->getWalletID();
        prop.sign(wallet_->getSecretKey());
        BlockProposalMessage *msg = new BlockProposalMessage(prop.getSize());
        msg->serialize(&prop);
        ConnectionManager::MessageRequest_t req = {.type=ConnectionManager::MessageRequestType::BROADCAST, .connectionID=0, .peerID="", .message=msg};
        connMng_->addMessageRequest(req);
    }

    bool ConsensusWrapper::addBlockProposalToChain(BlockProposalPtr &blockProp, byte64_t &currBlockId){
        BlockHeaderPtr blockhdr = std::make_shared<BlockHeader>();
        blockhdr->accountBalanceMerkleRootHash.SetNull();
        blockhdr->previousBlockHash = blockProp->proposedBlockHeader.previousBlockHash;
        blockhdr->sequence = blockProp->proposedBlockHeader.sequence;
        blockhdr->size = blockProp->proposedBlockHeader.size;
        blockhdr->transactionsMerkleRootHash = blockProp->proposedBlockHeader.transactionsMerkleRootHash;
        blockhdr->version = blockProp->proposedBlockHeader.version;
        chain_->insert(blockProp->issuer.getHex(), std::move(blockhdr));
        return chain_->updateValidBlock(currBlockId);
    }

    bool ConsensusWrapper::addBlockHeaderToChain(BlockPtr &block, byte64_t &currBlockId){
        BlockHeaderPtr blockhdr = std::make_shared<BlockHeader>();
        blockhdr->accountBalanceMerkleRootHash.SetNull();
        blockhdr->previousBlockHash = block->previousBlockHash;
        blockhdr->sequence = block->sequence;
        blockhdr->size = block->size;
        blockhdr->transactionsMerkleRootHash = block->transactionsMerkleRootHash;
        blockhdr->version = block->version;
        chain_->insert(wallet_->getWalletID().getHex(), std::move(blockhdr), true);
        return chain_->updateValidBlock(currBlockId);
    }

    void ConsensusWrapper::countAccountDifferencesByTxSet(AccountBalanceStorage *accBalanceStorage, Wallet *wallet, TransactionSet &txSet, std::unordered_map<std::string, AccountBalanceStorage::AccountDifference> &accDiffs){
        std::string thisNodeId = wallet->getWalletID().getHex();
        WalletData::TxState stateOfTx = WalletData::TxState::WAITING; // used in case of transaction related to local wallet
        AccountBalance acc;
        byte64_t currAcc; // Id of current processed account
        currAcc.SetNull();
        std::string senderHash;
        std::string receiverHash;
        PQB::cash upBalance = 0; // updated balance
        auto it = txSet.begin();
        while (it != txSet.end()){
            TransactionPtr tx = *it;
            receiverHash = tx->receiverWalletAddress.getHex();

            // Get balance data from account storage
            if (currAcc != tx->senderWalletAddress){
                if (accBalanceStorage->getBalance(tx->senderWalletAddress, acc)){
                    currAcc = tx->senderWalletAddress;
                    senderHash = tx->senderWalletAddress.getHex();
                    upBalance = acc.balance;
                } else {
                    ++it;
                    continue;
                }
            }

            bool thisNodeTxAsSender = false;
            bool thisNodeTxAsReceiv = false;
            if (tx->senderWalletAddress == wallet->getWalletID()){
                thisNodeTxAsSender = true;
            } else if (tx->receiverWalletAddress == wallet->getWalletID()){
                thisNodeTxAsReceiv = true;
            }

            // Peek for transaction sequence number, skip transactions from the same sender if they have the same sequence number
            bool flagMoved = false;
            auto next = it;
            ++next;
            while (next != txSet.end()){
                // if 2 transactions have the same seqence numbers -> skip them
                if ((currAcc == (*next)->senderWalletAddress) && (tx->sequenceNumber == (*next)->sequenceNumber)){
                    PQB_LOG_TRACE("CONSENSUS", "Transaction {} from sender {} not validated because of transactions with duplicit sequence numbers: {}",
                                shortStr(tx->IDHash.getHex()), shortStr(senderHash), tx->sequenceNumber);
                    flagMoved = true;
                    ++next;
                } else {
                    break;
                }
            }
            if (flagMoved){
                it = next;
                stateOfTx = WalletData::TxState::CANCELED;
                continue;
            }
            // Check for available balance
            if (upBalance < tx->cashAmount){ // check for available balance
                PQB_LOG_TRACE("CONSENSUS", "Transaction {} from sender {} not validated because of missing balance: {}", 
                            shortStr(tx->IDHash.getHex()), shortStr(senderHash), (upBalance - tx->cashAmount));
                stateOfTx = WalletData::TxState::CANCELED;
                ++it;
                continue;
            }
            // Check for sequence number, transaction sequence number has to be greater then last one
            if (acc.txSequence >= tx->sequenceNumber){ // check for sequence number
                PQB_LOG_TRACE("CONSENSUS", "Transaction {} from sender {} not validated because of invalid transaction sequence. Last sequence: {}, but this transaction has sequence: {}",
                            shortStr(tx->IDHash.getHex()), shortStr(senderHash), acc.txSequence, tx->sequenceNumber);
                stateOfTx = WalletData::TxState::CANCELED;
                ++it;
                continue;
            }
            PQB_LOG_TRACE("CONSENSUS", "Transaction {} from sender {} is validated", 
                        shortStr(tx->IDHash.getHex()), shortStr(senderHash));
            // Finish Balance difference
            stateOfTx = WalletData::TxState::CONFIRMED;
            upBalance -= tx->cashAmount;
            const auto senderIt = accDiffs.find(senderHash);
            if (senderIt != accDiffs.end()){
                senderIt->second.balanceDiff -= tx->cashAmount;
                senderIt->second.txSequence = tx->sequenceNumber;
            } else {
                accDiffs[senderHash] = {.id=&tx->senderWalletAddress, .balanceDiff=(0-tx->cashAmount), .txSequence=tx->sequenceNumber};
            }
            
            const auto receiverIt = accDiffs.find(receiverHash);
            if (receiverIt != accDiffs.end()){
                receiverIt->second.balanceDiff += tx->cashAmount;
            } else {
                accDiffs[receiverHash] = {.id=&tx->receiverWalletAddress, .balanceDiff=tx->cashAmount, .txSequence=0};
            }
            // Update local wallet
            if (thisNodeTxAsSender) { wallet->updateTransaction(tx->IDHash.getHex(), stateOfTx); }
            else if (thisNodeTxAsReceiv) { wallet->receivedTransaction(tx->IDHash.getHex(), tx->getTransactionData(), stateOfTx); }
            ++it;
        }
    }

    void ConsensusWrapper::countAccountDifferencesByTxSet(TransactionSet &txSet, std::unordered_map<std::string, AccountBalanceStorage::AccountDifference> &accDiffs){
        countAccountDifferencesByTxSet(accS_->blncDB, wallet_, txSet, accDiffs);
    }

    void ConsensusWrapper::executeBlock(BlockPtr block){
        std::unordered_map<std::string, AccountBalanceStorage::AccountDifference> accDiffs;
        countAccountDifferencesByTxSet(block->txSet, accDiffs);
        accS_->blncDB->setBalancesByAccDiffs(accDiffs);

        for (const auto &tx : block->txSet){
            txPool_.erase(tx->IDHash);
        }
        block->accountBalanceMerkleRootHash = accS_->blncDB->getAccountsMerkleRootHash();
        chain_->assignAccountHashToValidBlock(block->accountBalanceMerkleRootHash);
        blockS_->setBlock(block.get());
    }

    /*  CONSENSUS  */

    Consensus::Consensus(ConsensusWrapper *wrapper) : wrapper_(wrapper) {
        prevBlock_ = Chain::getGenesisBlock();
        currBlock_ = std::make_shared<Block>();
        prevBlockid_.setHex(std::string(GENESIS_BLOCK_HASH));
        currBlockId_.SetNull();
        prevRoundTime_ = std::chrono::milliseconds(15000);
        beginConsensus();
    }

    void Consensus::beginConsensus(){
        phase_ = ConsensusPhase::OPEN;
        result_.reset();
        converge_ = 0;
        openTime_ = Clock::now();
        currPeerProposals_.clear();
        acquiredSets_.clear();
        PQB_LOG_TRACE("CONSENSUS", "Consensus restarted");
    }

    void Consensus::timerEntry(){
        if (phase_ == ConsensusPhase::ACCEPTED){
            return;
        }

        auto [id, blockHdr] = wrapper_->getPreferred();
        if (id != prevBlockid_){
            // copy header to prevBlock_
            prevBlock_->setNull();
            prevBlock_->setBlockHeader(blockHdr.get());
            prevBlockid_ = id;
            beginConsensus();
        }

        PQB_LOG_TRACE("CONSENSUS", "Preferred block is: {} with sequence {}", shortStr(id.getHex()), prevBlock_->sequence);

        if (phase_ == ConsensusPhase::OPEN){
            if ((Clock::now() - openTime_) > (prevRoundTime_ / 2)){
                phase_ = ConsensusPhase::ESTABLISH;
                closeBlock();
            }
        } else if (phase_ == ConsensusPhase::ESTABLISH){
            result_.roundTime = Clock::now() - closeTime_;
            // roundTime_ / max(prevRoundTime_, 5s)
            converge_ = result_.roundTime / (Ms(5000) > prevRoundTime_ ? Ms(5000) : prevRoundTime_);
            updateProposals();
            if (haveConsensus()){
                phase_ = ConsensusPhase::ACCEPTED;
                onAccept();
            }
        }
    }

    void Consensus::closeBlock(){
        currBlock_->setNull();
        currBlock_->sequence = (prevBlock_->sequence + 1);
        wrapper_->getCurrentTransactionSet(result_.txns);
        // create proposal
        const auto time = std::chrono::system_clock::now();
        result_.txProposal.time = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
        result_.txProposal.previousBlockId = prevBlockid_;
        result_.txProposal.txSet.transactionCount = result_.txns.size();
        result_.txProposal.txSet.txSet = result_.txns;
        result_.txProposal.TxSetId = ComputeTxSetMerkleRoot(result_.txns);
        closeTime_ = Clock::now();
        wrapper_->share(result_.txProposal);
        result_.compares.clear();
        result_.disputes.clear();
        CTxSet newCTxSet = {.set=result_.txns, .setId=result_.txProposal.TxSetId, .time=result_.txProposal.time};
        acquiredSets_.emplace(result_.txProposal.TxSetId.getHex(), std::move(newCTxSet));
        for (const auto &pit :currPeerProposals_){
            const auto &pos = pit.second.txSetId;
            const auto &it = acquiredSets_.find(pos);
            if (it != acquiredSets_.end()){
                createDisputes(it->second);
            }
        }
        PQB_LOG_TRACE("CONSENSUS", "Block closed with transaction set {} and sequence {}", 
                    shortStr(result_.txProposal.TxSetId.getHex()), currBlock_->sequence);
    }

    void Consensus::gotTxSet(TxSetProposalPtr &prop){
        if (phase_ == ConsensusPhase::ACCEPTED){
            return;
        }
        if (prop->previousBlockId != prevBlockid_){
            return;
        }
        PeerId peerId = prop->issuer.getHex();
        auto peerPosIt = currPeerProposals_.find(peerId);
        if (peerPosIt != currPeerProposals_.end()){
            if (prop->seq <= peerPosIt->second.seq){
                return;
            }
        }

        TxSetId id = prop->TxSetId.getHex();
        CProposal newPos = {.prevBlockId=std::move(prop->previousBlockId), .txSetId=id, .time=prop->time, .seq=prop->seq};
        if (peerPosIt != currPeerProposals_.end()){
            peerPosIt->second = std::move(newPos);
        } else {
            currPeerProposals_.emplace(peerId, std::move(newPos));
        }

        PQB_LOG_TRACE("CONSENSUS", "Received transaction set {} from peer {}", shortStr(id), shortStr(peerId));

        CTxSet newCTxSet;
        const auto it = acquiredSets_.find(id);
        if (it != acquiredSets_.end()){ // set is already processed
            it->second.time = std::move(prop->time);
            return;
        } else {
            newCTxSet = {.set=std::move(prop->txSet.txSet), .setId=std::move(prop->TxSetId), .time=std::move(prop->time)};
            acquiredSets_.emplace(id, newCTxSet);
        }  

        if (!result_.isSet()){
            PQB_LOG_TRACE("CONSENSUS", "Not creating disputes: no position yet");
        } else {
            for (const auto &[nodeId, peerPos] : currPeerProposals_){
                if (peerPos.txSetId == id){
                    updateDisputes(nodeId, newCTxSet);
                }
            }
        }

    }

    void Consensus::updateProposals(){
        timestamp cTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        auto it = currPeerProposals_.begin();
        while (it != currPeerProposals_.end()){
            // if proposal is older then 20 seconds or more then it is stale proposal and it should be removed
            if ((cTime - it->second.time) >= 20){
                PQB_LOG_TRACE("CONSENSUS", "Removing stale proposal from peer {}", shortStr(it->first));
                for (auto &dt : result_.disputes){
                    dt.second.unVote(it->first);
                }
                it = currPeerProposals_.erase(it);
            } else{
                ++it;
            }
        }
        auto wit = acquiredSets_.begin();
        while (wit != acquiredSets_.end()){
            if ((cTime - wit->second.time) >= 20){
                PQB_LOG_TRACE("CONSENSUS", "Removing stale TxSet {}", shortStr(wit->first));
                wit = acquiredSets_.erase(wit);
            } else {
                ++wit;
            }
        }

        TransactionSet currSetCopy = result_.txns;
        for (auto &dt : result_.disputes){
            if (dt.second.updateVote(converge_)){
                if (dt.second.getOurVote()){
                    currSetCopy.insert(dt.second.getTx());
                } else {
                    currSetCopy.erase(dt.second.getTx());
                }
            }
        }
        if (currSetCopy != result_.txns){
            result_.txns = std::move(currSetCopy);
            const auto time = std::chrono::system_clock::now();
            result_.txProposal.time = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
            ++result_.txProposal.seq;
            result_.txProposal.previousBlockId = prevBlockid_;
            result_.txProposal.txSet.transactionCount = result_.txns.size();
            result_.txProposal.txSet.txSet = result_.txns;
            result_.txProposal.TxSetId = ComputeTxSetMerkleRoot(result_.txns);
            wrapper_->share(result_.txProposal);

            CTxSet newCTxSet = {.set=result_.txns, .setId=result_.txProposal.TxSetId, .time=result_.txProposal.time};
            if (acquiredSets_.emplace(result_.txProposal.TxSetId.getHex(), newCTxSet).second){
                for (const auto &[nodeId, peerPos] : currPeerProposals_){
                    if (peerPos.txSetId == result_.txProposal.TxSetId.getHex()){
                        updateDisputes(nodeId, newCTxSet);
                    }
                }
            }
            PQB_LOG_TRACE("CONSENSUS", "Proposed transaction set changed to {}", shortStr(result_.txProposal.TxSetId.getHex()));
        }
    }

    bool Consensus::haveConsensus(){
        size_t agree = 0;
        for (const auto &peer : currPeerProposals_){
            if (peer.second.txSetId == result_.txProposal.TxSetId.getHex()){
                ++agree;
            }
        }
        if (((agree + 1) / (wrapper_->getUNLcount() + 1)) > 0.8){
            PQB_LOG_TRACE("CONSENSUS", "Consensus reached on transaction set {}", shortStr(result_.txProposal.TxSetId.getHex()));
            return true;
        }
        return false;
    }

    void Consensus::onAccept(){
        // Finish new Block
        currBlock_->previousBlockHash = prevBlockid_;
        currBlock_->transactionCount = result_.txns.size();
        currBlock_->txSet = std::move(result_.txns);
        currBlock_->transactionsMerkleRootHash = result_.txProposal.TxSetId;
        currBlock_->size = currBlock_->getSize();
        currBlockId_ = currBlock_->getBlockHash();
        // Create proposal on this block
        BlockProposal bProp;
        bProp.blockId = currBlockId_;
        bProp.proposedBlockHeader = currBlock_->getBlockHeader();
        wrapper_->share(bProp);
        // Assign prevBlock and start new round
        *prevBlock_ = *currBlock_;
        prevBlockid_ = currBlockId_;
        prevRoundTime_ = result_.roundTime;
        PQB_LOG_TRACE("CONSENSUS", "New accepted block {}", shortStr(currBlockId_.getHex()));
        // Add block to chain, if new valid block is set execute it
        if (wrapper_->addBlockHeaderToChain(currBlock_, currBlockId_)){
            wrapper_->executeBlock(currBlock_);
            currBlock_->setNull(); // to save memory
        }
        beginConsensus();
    }

    void Consensus::peerProposal(BlockProposalPtr &prop){
        PQB_LOG_TRACE("CONSENSUS", "Received block proposal with transaction set {} and block sequence {} from peer {}", 
                    shortStr(prop->proposedBlockHeader.transactionsMerkleRootHash.getHex()), prop->proposedBlockHeader.sequence, shortStr(prop->issuer.getHex()));
        if (prop->proposedBlockHeader.sequence == currBlock_->sequence){
            if (wrapper_->addBlockProposalToChain(prop, currBlockId_)){
                wrapper_->executeBlock(currBlock_);
                currBlock_->setNull(); // to save memory
            }
        } else {
            PQB_LOG_TRACE("CONSENSUS", "Received block sequence is {}, but current block sequence for this node is {}",
                        prop->proposedBlockHeader.sequence, currBlock_->sequence);
        }
    }

    void Consensus::createDisputes(CTxSet &set){
        
        // Only create disputes if this is a new set
        if (!result_.compares.emplace(set.setId).second){
            return;
        }

        // Nothing to dispute if we agree
        if (set.setId == result_.txProposal.TxSetId){
            return;
        }

        TransactionSet diff;
        std::set_symmetric_difference(result_.txns.begin(), result_.txns.end(), set.set.begin(), set.set.end(), std::inserter(diff, diff.end()));
        for (auto &tx : diff){
            
            std::string txId = tx->IDHash.getHex();

            if (result_.disputes.find(txId) != result_.disputes.end()){
                continue;
            }

            bool isInMy = false;
            if (result_.txns.find(tx) != result_.txns.end()){
                isInMy = true;
            }

            DisputeTransaction dtx = DisputeTransaction(tx, isInMy, wrapper_->getUNLcount());

            for (const auto &[nodeId, pos] : currPeerProposals_){
                auto const cit = acquiredSets_.find(pos.txSetId);
                if (cit != acquiredSets_.end()){
                    dtx.setVote(nodeId, cit->second.exists(tx));
                }
            }
            result_.disputes.emplace(txId, std::move(dtx));
            PQB_LOG_TRACE("CONSENSUS", "Found disputed transaction {}", shortStr(txId));
        }
    }

    void Consensus::updateDisputes(const PeerId &node, CTxSet &set){
        if (result_.compares.find(set.setId) == result_.compares.end()){
            createDisputes(set);
        }

        for (auto &it : result_.disputes){
            auto &d = it.second;
            d.setVote(node, set.exists(d.getTx()));
        }
    }

} // namespace PQB

/* END OF FILE */