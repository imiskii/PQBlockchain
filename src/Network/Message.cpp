/**
 * @file Message.cpp
 * @author Michal Ľaš
 * @brief Messages in PQB blockchain
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Message.hpp"


namespace PQB{

    Message::Message(const message_hdr_t &messageHeader){
        msgHdr = messageHeader;
        currentMessageSize = 0;
        data.resize(msgHdr.size + getHeaderSize(), 0);
    }

    void Message::setRawData(byteBuffer &buffer){
        size_t copySize = (buffer.size() > msgHdr.size) ? msgHdr.size : buffer.size();
        std::memcpy(data.data() + getHeaderSize(), buffer.data(), copySize);
    }

    void Message::serializeMessageHeader(byteBuffer &buffer, size_t &offset, message_hdr_t &header)
    {
        if ((buffer.size() - offset) < getHeaderSize()){
            throw PQB::Exceptions::Message("Serialization: buffer has not enough size to serialize message header!");
        } 
        serializeField(buffer, offset, header.magicNum);
        serializeField(buffer, offset, header.type);
        serializeField(buffer, offset, header.size);
        serializeField(buffer, offset, header.checkSum);
    }

    void Message::deserializeMessageHeader(byteBuffer &buffer, size_t &offset, message_hdr_t &header){
        if ((buffer.size() - offset) < getHeaderSize()){
            throw PQB::Exceptions::Message("Deserialization: buffer has not enough size to deserialize message header!");
        }
        deserializeField(buffer, offset, header.magicNum);
        deserializeField(buffer, offset, header.type);
        deserializeField(buffer, offset, header.size);
        deserializeField(buffer, offset, header.checkSum);
    }

    void Message::serializeHeader(size_t &offset){
        serializeMessageHeader(data, offset, msgHdr);
    }

    void Message::deserializeHeader(size_t &offset){
        deserializeMessageHeader(data, offset, msgHdr);
    }

    void Message::addFragment(const char *fragment, size_t size){
        if (data.size() >= (currentMessageSize + size)){
            std::memcpy(data.data() + currentMessageSize, fragment, size);
            currentMessageSize += size;
        } else if (data.size() != currentMessageSize) {
            std::memcpy(data.data(), fragment, (data.size() - currentMessageSize));
            currentMessageSize = data.size();
        }
    }

    bool Message::checkMessage() const{
        if ((data.size() == currentMessageSize) && (msgHdr.magicNum == MESSAGE_MAGIC_CONST)){
            byte64_t messageHash;
            HashMan::SHA512_hash(&messageHash, data.data() + getHeaderSize(), getPayloadSize());
            // compare 32 bits of hash with check sum
            return (!std::memcmp(messageHash.data(), &msgHdr.checkSum, sizeof(msgHdr.checkSum)));
        }
        return false;
    }

    void Message::setCheckSum(){
        byte64_t messageHash;
        HashMan::SHA512_hash(&messageHash, data.data() + getHeaderSize(), getPayloadSize());
        std::memcpy(&msgHdr.checkSum, messageHash.data(), sizeof(msgHdr.checkSum));
        std::memcpy(data.data() + (getHeaderSize() - sizeof(msgHdr.checkSum)) , &msgHdr.checkSum, sizeof(msgHdr.checkSum));
    }

    /***** VERSION Message *****/

    void VersionMessage::serialize(void *messageStruct){
        version_msg_t *mData = static_cast<version_msg_t*>(messageStruct);
        size_t offset = 0;
        serializeHeader(offset);
        serializeField(data, offset, mData->version);
        serializeField(data, offset, mData->nodeType);
        serializeField(data, offset, mData->peerID);
    }

    void VersionMessage::deserialize(void *messageStruct) const{
        version_msg_t *mData = static_cast<version_msg_t*>(messageStruct);
        size_t offset = getHeaderSize();
        deserializeField(data, offset, mData->version);
        deserializeField(data, offset, mData->nodeType);
        deserializeField(data, offset, mData->peerID);
    }

    /***** ACK Message *****/

    void AckMessage::serialize(void *messageStruct){
        messageStruct = nullptr; // just to fill, there is no structure for ACK message
        size_t offset = 0;
        serializeHeader(offset);
        return;
    }

    void AckMessage::deserialize(void *messageStruct) const{
        messageStruct = nullptr; // just to fill, there is no structure for ACK message
        return;
    }

    /***** Block Message *****/

    void BlockMessage::serialize(void *messageStruct){
        Block *mData = static_cast<Block*>(messageStruct);
        size_t offset = 0;
        serializeHeader(offset);
        mData->serialize(data, offset);
    }

    void BlockMessage::deserialize(void *messageStruct) const{
        Block *mData = static_cast<Block*>(messageStruct);
        size_t offset = getHeaderSize();
        mData->deserialize(data, offset);
    }

    /***** Transaction Message *****/

    void TransactionMessage::serialize(void *messageStruct){
        Transaction *mData = static_cast<Transaction*>(messageStruct);
        size_t offset = 0;
        serializeHeader(offset);
        mData->serialize(data, offset);
    }

    void TransactionMessage::deserialize(void *messageStruct) const{
        Transaction *mData = static_cast<Transaction*>(messageStruct);
        size_t offset = getHeaderSize();
        mData->deserialize(data, offset);
    }

    /***** Account Message *****/

    void AccountMessage::serialize(void *messageStruct){
        Account *mData = static_cast<Account*>(messageStruct);
        size_t offset = 0;
        serializeHeader(offset);
        mData->serialize(data, offset);
    }

    void AccountMessage::deserialize(void *messageStruct) const{
        Account *mData = static_cast<Account*>(messageStruct);
        size_t offset = getHeaderSize();
        mData->deserialize(data, offset);
    }

    /***** Inventory Message *****/

    void InvMessage::serialize(void *messageStruct){
        std::vector<inv_message_t> *mData = static_cast<std::vector<inv_message_t>*>(messageStruct);
        size_t offset = 0;
        serializeHeader(offset);
        for (const auto &inv : *mData){
            serializeField(data, offset, inv.requestType);
            serializeField(data, offset, inv.itemID);
        }
    }

    void InvMessage::deserialize(void *messageStruct) const{
        std::vector<inv_message_t> *mData = static_cast<std::vector<inv_message_t>*>(messageStruct);
        size_t offset = getHeaderSize();
        // Message::getPayloadSize() get size of the payload in message, InvMessage::getPayloadSize(1) get size of one inventory
        size_t numInv = Message::getPayloadSize() / InvMessage::getPayloadSize(1);
        for (size_t i = 0; i < numInv; i++){
            inv_message_t inv;
            deserializeField(data, offset, inv.requestType);
            deserializeField(data, offset, inv.itemID);
            mData->push_back(inv);
        }
    }

    /***** GetData Message *****/

    void GetDataMessage::serialize(void *messageStruct){
        std::vector<inv_message_t> *mData = static_cast<std::vector<inv_message_t>*>(messageStruct);
        size_t offset = 0;
        serializeHeader(offset);
        for (const auto &inv : *mData){
            serializeField(data, offset, inv.requestType);
            serializeField(data, offset, inv.itemID);
        }
    }

    void GetDataMessage::deserialize(void *messageStruct) const{
        std::vector<inv_message_t> *mData = static_cast<std::vector<inv_message_t>*>(messageStruct);
        size_t offset = getHeaderSize();
        // Message::getPayloadSize() get size of the payload in message, InvMessage::getPayloadSize(1) get size of one inventory
        size_t numInv = Message::getPayloadSize() / GetDataMessage::getPayloadSize(1);
        for (size_t i = 0; i < numInv; i++){
            inv_message_t inv;
            deserializeField(data, offset, inv.requestType);
            deserializeField(data, offset, inv.itemID);
            mData->push_back(inv);
        }
    }

    /***** BlockProposal Message *****/

    void BlockProposalMessage::serialize(void *messageStruct){
        BlockProposal *mData = static_cast<BlockProposal*>(messageStruct);
        size_t offset = 0;
        serializeHeader(offset);
        mData->serialize(data, offset);
    }

    void BlockProposalMessage::deserialize(void *messageStruct) const{
        BlockProposal *mData = static_cast<BlockProposal*>(messageStruct);
        size_t offset = getHeaderSize();
        mData->deserialize(data, offset);
    }

    /***** TxSetProposal Message *****/

    void TxSetProposalMessage::serialize(void *messageStruct){
        TxSetProposal *mData = static_cast<TxSetProposal*>(messageStruct);
        size_t offset = 0;
        serializeHeader(offset);
        mData->serialize(data, offset);
    }

    void TxSetProposalMessage::deserialize(void *messageStruct) const{
        TxSetProposal *mData = static_cast<TxSetProposal*>(messageStruct);
        size_t offset = getHeaderSize();
        mData->deserialize(data, offset);
    }

    /***** MessageCreator *****/

    namespace MessageCreator
    {
        
        Message* createMessage(Message::message_hdr_t &msgHeader){
            switch (msgHeader.type)
            {
            case MessageType::VERSION:
                return new VersionMessage(msgHeader);
            case MessageType::ACK:
                return new AckMessage(msgHeader);
            case MessageType::TX:
                return new TransactionMessage(msgHeader);
            case MessageType::BLOCKPROPOSAL:
                return new BlockProposalMessage(msgHeader);
            case MessageType::TXSETPROPOSAL:
                return new TxSetProposalMessage(msgHeader);
            case MessageType::BLOCK:
                return new BlockMessage(msgHeader);
            case MessageType::ACCOUNT:
                return new AccountMessage(msgHeader);
            case MessageType::INV:
                return new InvMessage(msgHeader);
            case MessageType::GETDATA:
                return new GetDataMessage(msgHeader);
            default:
                break;
            }
            return nullptr;
        }

    } // namespace MessageCreator


} // namespace PQB

/* END OF FILE */