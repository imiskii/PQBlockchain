/**
 * @file ConsensusParams.hpp
 * @author Michal Ľaš
 * @brief Types and parameters for Consensus
 * @date 2024-03-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once


#include <chrono>
#include <string>
#include <unordered_map>
#include "Proposal.hpp"
#include "Transaction.hpp"


namespace PQB{


class DisputeTransaction{
private:
    TransactionPtr tx_;  ///< disputed transaction
    bool ourVote_;       ///< whether transaction should be included
    std::uint32_t yays_; ///< number of yes from other nodes
    std::uint32_t nays_; ///< number of no from other nodes
    std::unordered_map<std::string, bool> votes_; ///< collection of votes indexed by node

public:

    DisputeTransaction(const TransactionPtr &transaction, bool ourVote, size_t numPeers)
    : tx_(transaction), ourVote_(ourVote), yays_(0), nays_(0){
        votes_.reserve(numPeers);
    }

    bool getOurVote(){
        return ourVote_;
    }

    TransactionPtr getTx(){
        return tx_;
    }

    /// @brief Set `vote` of peer with given `peerId`
    void setVote(const std::string &nodeId, bool vote);

    void unVote(const std::string &nodeId);

    /// @brief Update our vote base on `converge` ration
    /// @param converge represents stage of consensus. In late stages there is higher requirments to achieve agreement.
    /// @return true if ourVote was changed, false if ourVote was not changed
    bool updateVote(double &converge);

};


enum class ConsensusPhase{
    OPEN,
    ESTABLISH,
    ACCEPTED
};


/// @brief Consensus proposal
struct CProposal{
    byte64_t prevBlockId; ///< last proposed block ID
    std::string txSetId;  ///< last proposed transaction set ID
    timestamp time;       ///< proposed time
    uint32_t seq;         ///< sequence number of the last proposal
};

/// @brief Transaction set for consensus
struct CTxSet{
    TransactionSet set;
    timestamp time;    ///< last time when this set was proposed

    /// @brief Check if given transaction exists in this set
    /// @warning Transaction is searched by pointer because all transaction appear in program just once
    bool exists(const TransactionPtr &txPtr);
};

class ConsensusResult{
public:

    TransactionSet txns; ///< set of transactions consensus agrees on
    TxSetProposal txProposal;
    std::unordered_map<std::string, DisputeTransaction> disputes; ///< collection of disputed transactios
    std::chrono::duration<double, std::milli> roundTime; ///< duration of the establish phase


    ConsensusResult(){
        reset();
    }

    void reset(){
        txns.clear();
        txProposal.setNull();
        disputes.clear();
        roundTime = std::chrono::milliseconds(0);
    }

};




} // namespace PQB

/* END OF FILE */