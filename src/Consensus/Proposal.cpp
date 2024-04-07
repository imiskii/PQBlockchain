/**
 * @file Proposal.cpp
 * @author Michal Ľaš
 * @brief Implementation of Consensus proposal
 * @date 2024-03-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Proposal.hpp"


namespace PQB{

    bool BlockProposal::check(){
        if (issuer.IsNull() ||
            blockId.IsNull() ||
            proposedBlockHeader.previousBlockHash.IsNull() ||
            proposedBlockHeader.transactionsMerkleRootHash.IsNull() ||
            !proposedBlockHeader.accountBalanceMerkleRootHash.IsNull() ||
            proposedBlockHeader.sequence == 0){
                return false;
            }
        return true;
    }

    void BlockProposal::getHash(byte64_t &outHash) const{
        byteBuffer dataToHash;
        size_t offset = 0;
        dataToHash.resize(sizeof(typeOfProposal) + sizeof(issuer) + sizeof(blockId));
        serializeField(dataToHash, offset, typeOfProposal);
        serializeField(dataToHash, offset, issuer);
        serializeField(dataToHash, offset, blockId);
        HashMan::SHA512_hash(&outHash, dataToHash.data(), dataToHash.size());
    }

    void BlockProposal::sign(byteBuffer &privateKey){
        byte64_t propHash;
        getHash(propHash);
        SignAlgorithmPtr s = Signer::GetInstance();
        signatureSize = s->sign(signature, propHash.data(), propHash.size(), privateKey);
    }

    bool BlockProposal::verify(byteBuffer &publicKey){
        byte64_t propHash;
        getHash(propHash);
        SignAlgorithmPtr s = Signer::GetInstance();
        if (s->verify(signature, propHash.data(), propHash.size(), publicKey)){
            return true;
        }
        return false;
    }

    size_t BlockProposal::getSize() const{
        if (signatureSize == 0){
            throw Exceptions::Proposal("Proposal can not be serialized because proposal is not signed and size of the signature is unknow!");
        }
        return sizeof(typeOfProposal) +
               sizeof(issuer) +
               sizeof(blockId) +
               sizeof(signatureSize) +
               signatureSize +
               proposedBlockHeader.getSize();
    }

    void BlockProposal::serialize(byteBuffer &buffer, size_t &offset) const{
        if ((buffer.size() - offset) < getSize())
            throw PQB::Exceptions::Proposal("Serialization: serialization buffer has not enough size to serialize the proposal");
        serializeField(buffer, offset, typeOfProposal);
        serializeField(buffer, offset, issuer);
        serializeField(buffer, offset, blockId);
        serializeField(buffer, offset, signatureSize);
        std::memcpy(buffer.data() + offset, signature.data(), signatureSize);
        offset += signatureSize;
        /// Skip checks because there is missing account hash -> it will be filled after the consensus
        proposedBlockHeader.serialize(buffer, offset, true);
    }

    void BlockProposal::deserialize(const byteBuffer &buffer, size_t &offset){
        if ((buffer.size() - offset) < (sizeof(typeOfProposal) + sizeof(issuer) + sizeof(blockId) + sizeof(signatureSize)))
            throw PQB::Exceptions::Transaction("Deserialization: buffer has not enough size to deserialize the proposal");
        deserializeField(buffer, offset, typeOfProposal);
        deserializeField(buffer, offset, issuer);
        deserializeField(buffer, offset, blockId);
        deserializeField(buffer, offset, signatureSize);
        if ((buffer.size() - offset) < signatureSize)
            throw PQB::Exceptions::Transaction("Deserialization: buffer has not enough size to deserialize the proposal");
        signature.resize(signatureSize);
        std::memcpy(signature.data(), buffer.data() + offset, signatureSize);
        offset += signatureSize;
        proposedBlockHeader.deserialize(buffer, offset);
    }



    bool TxSetProposal::check(){
        if (issuer.IsNull() ||
            time == 0 ||
            TxSetId.IsNull() ||
            previousBlockId.IsNull()){
                return false;
            }
        return true;
    }

    void TxSetProposal::getHash(byte64_t &outHash) const
    {
        byteBuffer dataToHash;
        size_t offset = 0;
        dataToHash.resize(sizeof(typeOfProposal) + sizeof(seq) + sizeof(time) + sizeof(issuer) + sizeof(TxSetId) + sizeof(previousBlockId));
        serializeField(dataToHash, offset, typeOfProposal);
        serializeField(dataToHash, offset, seq);
        serializeField(dataToHash, offset, time);
        serializeField(dataToHash, offset, issuer);
        serializeField(dataToHash, offset, TxSetId);
        serializeField(dataToHash, offset, previousBlockId);
        HashMan::SHA512_hash(&outHash, dataToHash.data(), dataToHash.size());
    }

    void TxSetProposal::sign(byteBuffer &privateKey){
        byte64_t propHash;
        getHash(propHash);
        SignAlgorithmPtr s = Signer::GetInstance();
        signatureSize = s->sign(signature, propHash.data(), propHash.size(), privateKey);
    }

    bool TxSetProposal::verify(byteBuffer &publicKey){
        byte64_t propHash;
        getHash(propHash);
        SignAlgorithmPtr s = Signer::GetInstance();
        if (s->verify(signature, propHash.data(), propHash.size(), publicKey)){
            return true;
        }
        return false;
    }

    size_t TxSetProposal::getSize() const{
        if (signatureSize == 0){
            throw Exceptions::Proposal("Proposal can not be serialized because proposal is not signed and size of the signature is unknow!");
        }
        return sizeof(typeOfProposal) +
               sizeof(seq) +
               sizeof(time) +
               sizeof(issuer) +
               sizeof(TxSetId) +
               sizeof(previousBlockId) +
               sizeof(signatureSize) +
               signatureSize +
               txSet.getSize();
    }

    void TxSetProposal::serialize(byteBuffer &buffer, size_t &offset) const{
        if ((buffer.size() - offset) < getSize())
            throw PQB::Exceptions::Proposal("Serialization: serialization buffer has not enough size to serialize the proposal");
        serializeField(buffer, offset, typeOfProposal);
        serializeField(buffer, offset, seq);
        serializeField(buffer, offset, time);
        serializeField(buffer, offset, issuer);
        serializeField(buffer, offset, TxSetId);
        serializeField(buffer, offset, previousBlockId);
        serializeField(buffer, offset, signatureSize);
        std::memcpy(buffer.data() + offset, signature.data(), signatureSize);
        offset += signatureSize;
        txSet.serialize(buffer, offset);
    }

    void TxSetProposal::deserialize(const byteBuffer &buffer, size_t &offset){
        if ((buffer.size() - offset) < (sizeof(typeOfProposal) + sizeof(seq) + sizeof(time) + sizeof(issuer) + sizeof(TxSetId) + sizeof(previousBlockId) + sizeof(signatureSize)))
            throw PQB::Exceptions::Transaction("Deserialization: buffer has not enough size to deserialize the proposal");
        deserializeField(buffer, offset, typeOfProposal);
        deserializeField(buffer, offset, seq);
        deserializeField(buffer, offset, time);
        deserializeField(buffer, offset, issuer);
        deserializeField(buffer, offset, TxSetId);
        deserializeField(buffer, offset, previousBlockId);
        deserializeField(buffer, offset, signatureSize);
        if ((buffer.size() - offset) < signatureSize)
            throw PQB::Exceptions::Transaction("Deserialization: buffer has not enough size to deserialize the proposal");
        signature.resize(signatureSize);
        std::memcpy(signature.data(), buffer.data() + offset, signatureSize);
        offset += signatureSize;
        txSet.deserialize(buffer, offset);
    }

    void TxSetProposal::deserializeExceptTxSet(const byteBuffer &buffer, size_t &offset){
        if ((buffer.size() - offset) < (sizeof(typeOfProposal) + sizeof(seq) + sizeof(time) + sizeof(issuer) + sizeof(TxSetId) + sizeof(previousBlockId) + sizeof(signatureSize)))
            throw PQB::Exceptions::Transaction("Deserialization: buffer has not enough size to deserialize the proposal");
        deserializeField(buffer, offset, typeOfProposal);
        deserializeField(buffer, offset, seq);
        deserializeField(buffer, offset, time);
        deserializeField(buffer, offset, issuer);
        deserializeField(buffer, offset, TxSetId);
        deserializeField(buffer, offset, previousBlockId);
        deserializeField(buffer, offset, signatureSize);
        if ((buffer.size() - offset) < signatureSize)
            throw PQB::Exceptions::Transaction("Deserialization: buffer has not enough size to deserialize the proposal");
        signature.resize(signatureSize);
        std::memcpy(signature.data(), buffer.data() + offset, signatureSize);
        offset += signatureSize;
        deserializeField(buffer, offset, txSet.transactionCount);
    }

} // namespace PQB

/* END OF FILE */