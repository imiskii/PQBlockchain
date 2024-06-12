/**
 * @file Consensus.hpp
 * @author Michal Ľaš
 * @brief Implementation of the Ripple XRP consensus mechanism
 * @date 2024-03-17
 * 
 * @copyright Copyright (c) 2024
 * 
 * 
 * This code was inspired by original implementation of the Ripple XRL consensus mechanism:
 * @author Ripple Labs Inc.
 * Availability: https://github.com/XRPLF/rippled/blob/develop/src/ripple/consensus
 * 
 * and Article:
 * 
 * title: Security Analysis of Ripple Consensus
 * author: Ignacio Amores-Sesar, Christian Cachin, Jovana Micic
 * eprinttype: arXiv
 * journal: CoRR
 * year: 2020
 * url: https://arxiv.org/abs/2011.14816
 * 
 * @copyright (c) 2012-2017
 * 
 */


#pragma once

#include <map>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <utility>
#include "PQBconstants.hpp"
#include "MerkleRootCompute.hpp"
#include "BlocksStorage.hpp"
#include "AccountStorage.hpp"
#include "Account.hpp"
#include "Wallet.hpp"
#include "ConsensusParams.hpp"
#include "PQBtypedefs.hpp"
#include "Blob.hpp"
#include "Transaction.hpp"
#include "Block.hpp"
#include "Chain.hpp"

namespace PQB{

class ConnectionManager;
class Consensus;

class ConsensusWrapper{

friend class Consensus;

private:

    Chain *chain_;
    BlocksStorage *blockS_;
    AccountStorage *accS_;
    Wallet *wallet_;
    ConnectionManager *connMng_;

    TransactionPool txPool_;
    Consensus *consensus_;
    std::jthread consensusThread_;
    std::condition_variable consensusCondition_;
    std::mutex consensusMutex_;
    std::atomic_bool consensusRun_; ///< flag indicating if consensus should run

    /// @brief Thread which is running loop with consensus
    void consensusThread();

    /// @brief Broadcast transaction set proposal to peers, this will also set the signature of the proposal
    void share(TxSetProposal &prop);

    /// @brief Broadcast block proposal to peers, this will also set the signature of the proposal
    void share(BlockProposal &prop);

    /// @brief Add Block header to Chain, after insertion checks if consensus on block with `currBlockId` ID was reached
    /// @warning this may change valid block, check return value
    /// @param blockProp proposal with block header that will be inserted into chain
    /// @param currBlockId id of current working block
    /// @return true if quorum for consensus on block validation was reached, false if not
    bool addBlockProposalToChain(BlockProposalPtr &blockProp, byte64_t &currBlockId);

    /// @brief Add Block header to Chain (header issuer is local node)
    /// @warning this method should be used to add block headers that was issued by this node only
    /// @param block block with block header that will be inserted into chain
    /// @param currBlockId id of current working block (hash of `block`)
    /// @return true if quorum for consensus on block validation was reached, false if not
    bool addBlockHeaderToChain(BlockPtr &block, byte64_t &currBlockId);

    /// @brief Iterate through given `txSet`, check the sequence numbers of the transactions, balances of accounts and fill  
    /// `accDiffs` hash table with account differences. This also notify wallet and set wallet account balance and transaction records.
    /// Additionaly this function removes invalid transactions from txSet.
    /// @param txSet set with transaction for which will be created account differences
    /// @param accDiffs [out] filled account difference hash table
    /// @note This function depends on right ordering of transactions in given `txSet`. The transactions have to be
    /// grouped by senderId and then ordered by transactions sequence numbers.
    void countAccountDifferencesByTxSet(
        TransactionSet &txSet, 
        std::unordered_map<std::string, AccountBalanceStorage::AccountDifference> &accDiffs);

    /// @brief @brief Execute block transaction, remove all executed transactions from txPool,
    /// update account balances, and add block to storage
    /// @param block block to execute
    void executeBlock(BlockPtr block);

    /// @brief Get ID and header of Block on which majority of UNL nodes are woking on
    std::pair<byte64_t&, BlockHeaderPtr&> getPreferred();

    /// @brief Fill `set` with transactions (transactions from txPool_)
    /// @param set [out] final set of transactions
    void getCurrentTransactionSet(TransactionSet &set);

    /// @brief Get number of UNL nodes
    size_t getUNLcount();


public:

    ConsensusWrapper(Chain *chain, BlocksStorage *blockS, AccountStorage *accS, Wallet *wallet);

    ~ConsensusWrapper();

    /// @brief Assign Message Processor to this ConsensusWrapper
    void assignConnectionManager(ConnectionManager *connMng){
        connMng_ = connMng;
    }

    /// @brief Check if given transaction is in Transaction pool
    bool isTransactionInPool(const byte64_t tx_id);

    /// @brief Get transaction from pool based on give transaction has (ID), return nullptr if not found
    TransactionPtr getTransactionFromPool(const byte64_t tx_id);

    /// @brief Add new transaction to the transaction pool
    /// @return true if transaction was added, false if transaction already exists in the pool
    bool addTransactionToPool(TransactionPtr tx);

    /// @brief Erase transaction from transaction pool, if not found do nothing
    void removeTransactionFromPool(const byte64_t tx_id);

    /// @brief Notify ConsensusWrapper that new BlockProposal was received
    void notifyBlockProposal(BlockProposalPtr &prop);

    /// @brief Notify ConsensusWrapper that new TransactionSetProposal was received
    void notifyTxSetProposal(TxSetProposalPtr &prop);

    static void countAccountDifferencesByTxSet(
    AccountBalanceStorage *accBalanceStorage, 
    Wallet *wallet,
    TransactionSet &txSet, 
    std::unordered_map<std::string, AccountBalanceStorage::AccountDifference> &accDiffs);

};



class Consensus{
private:

    using PeerId = std::string;
    using TxSetId = std::string;
    using BlockId = byte64_t;
    using Ms = std::chrono::milliseconds;
    using Clock = std::chrono::steady_clock;
    using TimePoint = std::chrono::time_point<Clock>;
    using TimeInterval = std::chrono::duration<double, std::milli>;

    ConsensusPhase phase_;
    BlockPtr currBlock_; ///< current working block
    BlockId currBlockId_;
    BlockPtr prevBlock_; ///< last agreed-on ("closed") block according to the network
    BlockId prevBlockid_;
    ConsensusResult result_; ///< data relevant for the outcome of consensus on a single ledger
    std::unordered_map<PeerId, CProposal> currPeerProposals_;
    std::unordered_map<TxSetId, CTxSet> acquiredSets_;
    double converge_;   ///< ration of round time to prevRoundTime

    TimeInterval prevRoundTime_; ///< time taken by the previous consensus round, initialized to 15s
    TimePoint openTime_;        ///< time when the last open phase started
    TimePoint closeTime_;       ///< time when the last establish phase started
    ConsensusWrapper *wrapper_;

    //void startRoundInternal();

public:

    Consensus(ConsensusWrapper *wrapper);    

    /// @brief Restart consensus attributes
    void beginConsensus();

    /// @brief Do one step of consensus
    void timerEntry();

    /// @brief Take current transactions, place them to a block and move to establish phase
    void closeBlock();

    /// @brief Process a transaction set (`prop`) by the consensus
    void gotTxSet(TxSetProposalPtr &prop);

    /// @brief Process a block proposal (`prop`) by the consensus
    void peerProposal(BlockProposalPtr &prop);

    /// @brief Create hash map of disputed transactions (it is stored in result_->disputes)
    /// Disputes are transactions that are different between proposed set of transactions
    /// @param set Set of transaction for which to create disputes
    void createDisputes(CTxSet &set);

    /// @brief Update disputes based on new proposed `set` from some `node`
    /// @param node node for which to update disputes
    /// @param set set of proposed transactions
    void updateDisputes(const PeerId &node, CTxSet &set);

    /// @brief Base on disputed transaction update our proposal
    void updateProposals();

    /// @brief Determine if we already have the consensus on a set of transactions
    bool haveConsensus();

    /// @brief Move to accept phase. The consensus was reached. Propose new block proposal and move to another round of consensus
    void onAccept();

    
};

} // namespace PQB

/* END OF FILE */