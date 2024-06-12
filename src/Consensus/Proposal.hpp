/**
 * @file Proposal.hpp
 * @author Michal Ľaš
 * @brief Implementation of Consensus proposal
 * @date 2024-03-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <set>
#include <memory>
#include "PQBtypedefs.hpp"
#include "Block.hpp"
#include "Signer.hpp"
#include "HashManager.hpp"
#include "Transaction.hpp"
#include "Blob.hpp"
#include "PQBtypedefs.hpp"
#include "Serialize.hpp"


namespace PQB{


enum class ProposalType{
    BLOCK,
    TxSet
};


class BlockProposal{
public:

    ProposalType typeOfProposal{ProposalType::BLOCK};
    byte64_t issuer;  ///< identification of the proposal creator
    byte64_t blockId; ///< identification of this new proposed block

    uint32_t signatureSize;
    std::vector<PQB::byte> signature; ///< signature of the issuer

    BlockHeader proposedBlockHeader;

    BlockProposal(){
        setNull();
    }

    void setNull(){
        typeOfProposal = ProposalType::BLOCK;
        issuer.SetNull();
        blockId.SetNull();
        signatureSize = 0;
        signature.clear();
        proposedBlockHeader.setNull();
    }

    /// @brief Check structure of the proposal
    bool check();

    /// @brief Get hash of this proposal
    /// @param outHash [out] output hash
    void getHash(byte64_t &outHash) const;

    /// @brief sign the proposal
    void sign(byteBuffer &privateKey);

    /// @brief verify the proposal signature
    bool verify(byteBuffer &publicKey);

    /// @brief Get size of the proposal object
    /// @exception if proposal is not signed so size of signature is unknow
    size_t getSize() const;

    /// @brief Serialize BlockProposal
    /// @param buffer buffer for serialization
    /// @param offset offset to the buffer
    /// @exception if buffer has not enough size for the proposal serialization
    void serialize(byteBuffer &buffer, size_t &offset) const;

    /// @brief Deserialize BlockProposal
    /// @param buffer buffer with serialized data
    /// @param offset offset to the buffer
    /// @exception if buffer has not enough size for the proposal deserialization
    void deserialize(const byteBuffer &buffer, size_t &offset);
};


class TxSetProposal{
public:

    ProposalType typeOfProposal{ProposalType::TxSet};
    uint32_t seq;     ///< sequence number of the proposal, unique for user, initialzed to 0 at the start of each round
    timestamp time;   ///< time of proposal creation
    byte64_t issuer;  ///< identification of the proposal creator
    byte64_t TxSetId; ///< identification of proposed transaction set (Merkle root hash of the transaction set)
    byte64_t previousBlockId; ///< identification of previous Block on which this proposal is based on

    uint32_t signatureSize;
    std::vector<PQB::byte> signature; ///< signature of the issuer

    BlockBody txSet; ///< Body of the block consist of number of transaction and the transaction set

    TxSetProposal(){
        setNull();
    }

    void setNull(){
        typeOfProposal = ProposalType::TxSet;
        seq = 0;
        time = 0;
        issuer.SetNull();
        TxSetId.SetNull();
        previousBlockId.SetNull();
        signatureSize = 0;
        signature.clear();
        txSet.setNull();
    }

    /// @brief Check structure of the proposal
    bool check();

    /// @brief Get hash of this proposal
    /// @param outHash [out] output hash
    void getHash(byte64_t &outHash) const;

    /// @brief sign the proposal
    void sign(byteBuffer &privateKey);

    /// @brief verify the proposal signature
    bool verify(byteBuffer &publicKey);

    /// @brief Get size of the proposal object
    /// @exception if proposal is not signed so size of signature is unknow
    size_t getSize() const;

    /// @brief Serialize TxSetProposal
    /// @param buffer buffer for serialization
    /// @param offset offset to the buffer
    /// @exception if buffer has not enough size for the proposal serialization
    void serialize(byteBuffer &buffer, size_t &offset) const;

    /// @brief Deserialize TxSetProposal
    /// @param buffer buffer with serialized data
    /// @param offset offset to the buffer
    /// @exception if buffer has not enough size for the proposal deserialization
    void deserialize(const byteBuffer &buffer, size_t &offset);

    /// @brief Deserialize TxSetProposal data except the TxSet
    /// @param buffer buffer with serialized data
    /// @param offset offset to the buffer
    /// @exception if buffer has not enough size for the proposal deserialization
    void deserializeExceptTxSet(const byteBuffer &buffer, size_t &offset);

};


typedef std::shared_ptr<BlockProposal> BlockProposalPtr;
typedef std::shared_ptr<TxSetProposal> TxSetProposalPtr;

} // namespace PQB

/* END OF FILE */