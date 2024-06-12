/**
 * @file ConsensusParams.cpp
 * @author Michal Ľaš
 * @brief Types and parameters for Consensus
 * @date 2024-03-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "ConsensusParams.hpp"


namespace PQB{

    void DisputeTransaction::setVote(const std::string &nodeId, bool vote){
        auto const [it, inserted] = votes_.insert(std::make_pair(nodeId, vote));
        if (inserted){
            if (vote){
                ++yays_;
            } else {
                ++nays_;
            }
        } else if (vote && !it->second){
            --nays_;
            ++yays_;
            it->second = true;
        } else if (!vote && it->second){
            ++nays_;
            --yays_;
            it->second = false;
        }
    }

    void DisputeTransaction::unVote(const std::string &nodeId){
        auto it = votes_.find(nodeId);
        if (it != votes_.end()){
            if (it->second){
                --yays_;
            } else {
                --nays_;
            }
            votes_.erase(it);
        }
    }

    bool DisputeTransaction::updateVote(double &converge){
        double trashold = 0;
        if (converge < 0.5){
            trashold = 0.5;
        } else if (converge < 0.85){
            trashold = 0.65;
        } else if (converge < 2){
            trashold = 0.7;
        } else {
            trashold = 0.95;
        }
        bool newVote = ((yays_ + (ourVote_ ? 1 : 0)) / (yays_ + nays_ + 1)) > trashold;
        if (newVote != ourVote_){
            ourVote_ = newVote;
            return true;
        }
        return false;
    }

    bool CTxSet::exists(const TransactionPtr &txPtr){
        if (set.find(txPtr) != set.end()){
            return true;
        }
        return false;
    }

} // namespace PQB

/* END OF FILE */