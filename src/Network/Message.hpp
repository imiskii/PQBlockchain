/**
 * @file Message.hpp
 * @author Michal Ľaš
 * @brief Messages in PQB blockchain
 * @date 2024-02-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#pragma once

#include <stdint.h>
#include <ctime>
#include <memory>
#include "PQBtypedefs.hpp"
#include "PQBExceptions.hpp"
#include "Blob.hpp"
#include "Transaction.hpp"
#include "Block.hpp"
#include "HashManager.hpp"
#include "Serialize.hpp"
#include "Account.hpp"

namespace PQB{


/// @brief Type of messages - the value of the enmu represent importatnce of the message. Message with higher importance are processed with priority.
/// For example BLOCK, TX, and ACCOUNT messages are for more important
enum class MessageType : uint32_t{
    // @brief Describes the peer which is asking for connection
    VERSION = 0,
    /// @brief Replay to peer which send the VERSION message. If other side refuse the connection then this connection is just close.
    /// There is no need for special message as negative ACK.
    ACK = 1,
    /// @brief  Message with one transaction in payload.
    TX = 100,
    /// @todo fill
    PROPOSAL = 101,
    /// @brief  Message with one account in payload.
    ACCOUNT = 102,
    /// @brief  Message with one block in payload.
    BLOCK = 103,
    /// @brief  Message with multiple inventories. Inventory is pair of inventory type and identifier of item.
    /// This message is used to offer to some peer a data about for example block, transaction or account
    INV = 50,
    /// @brief This message has the same structure as INV message. And it is used to request some peer for data given in inventory.
    GETDATA = 51,
    /// @brief Message requesting accounts. It consists of recent accounts merkle root hash which the requesting peer has.
    /// If the peer does not have any accounts it uses account merke root hash from the genesis block
    // GETACCOUNTS = 52,
    /// @brief Message for requesting block inventories. It consists of last block hash which the requesting peer has and number of
    /// following block it is requesting (0 means all). If peer has no blocks yet this field has hash of the genesis block.
    // GETBLOCKS = 53

};

/// @brief Type of inventories
enum class InvType : uint32_t{
    BLOCK,
    TX,
    ACCOUNT
};

struct inv_message_t{
    InvType requestType;    ///< Type of request, specify what item is requested with this message
    byte64_t itemID;        ///< unique identifier of the requested item
};


/// @brief first 32 bits of SHA-512 hash of empty string. This constant is used as check if message header was parsed successfully
const uint32_t MESSAGE_MAGIC_CONST = 4178435447;

class Message{
public:

    struct message_hdr_t{
        uint32_t magicNum;  ///< Number for checking if header was well parsed
        MessageType type;   ///< Type of the message
        uint32_t size;      ///< Size of message payload (without header size !)
        uint32_t checkSum;  ///< Check sum of the message (first 32 bits of SHA-512 hash of the message). This field has to stay at last position in message_hdr_t struct!
        // DO NOT ADD OTHER FIELDS HERE. checkSum has to stay as last field of message_hdr_t struct!
    };

    Message(const message_hdr_t &messageHeader);
    virtual ~Message() {}

    /// @brief get pointer to message data (with header)
    PQB::byte *getData(){
        return data.data();
    }

    /// @brief get size of message data (with header) in bytes
    size_t getSize() const {
        return data.size();
    }

    /// @brief Get size of the message header in bytes 
    static size_t getHeaderSize(){
        return sizeof(uint32_t) * 4; // sizeof(magicNum) + sizeof(type) + sizeof(size) + sizeof(checksum)
    }

    /// @brief Get size of message data without header
    size_t getPayloadSize() const {
        return (data.size() - getHeaderSize());
    }

    /// @brief get type of the message (see enum class MessageType)
    MessageType getType() const {
        return msgHdr.type;
    }

    /// @brief get header of this message
    message_hdr_t getMessageHeader() const {
        return msgHdr;
    }

    /// @brief Set bytes from buffer to message data (payload).
    /// if buffer is larger the message data, set just message data size bytes
    void setRawData(byteBuffer &buffer);

    /**
     * @brief Serialize message header to a buffer
     * 
     * @param buffer [out] buffer for serialization
     * @param offset offset to the buffer
     * @param header header to serialize
     * @exception if buffer has not enough size for serialization
     */
    static void serializeMessageHeader(byteBuffer &buffer, size_t &offset, message_hdr_t &header);

    /**
     * @brief Deserialize message header from a buffer
     * 
     * @param buffer buffer with serialized header data
     * @param offset offset to the buffer
     * @param header [out] deserialized message header
     * @exception if buffer has not enough size to deserialize message header
     */
    static void deserializeMessageHeader(byteBuffer &buffer, size_t &offset, message_hdr_t &header);

    /// @brief Serialize message header to data buffer
    /// @param offset
    void serializeHeader(size_t &offset);

    /// @brief eserialize message header from data buffer
    /// @param offset 
    void deserializeHeader(size_t &offset);

    /**
     * @brief Add part of the message at the end of current message. At start iterator is at the begining of data.
     * When reciving message fragments they should also include the header of the message, so it will be also included in data of the message.
     * 
     * @param fragment pointer to message data
     * @param size size of the data
     */
    void addFragment(const char* fragment, size_t size);

    /// @brief Check message size, MAGIC_CONST and the checkSum (message data has to be complete before checking else it returns false)
    bool checkMessage() const;

    /// @brief set the check sum of this message (check sum is 32 bits of message data hash)
    void setCheckSum();

    /**
     * @brief Serialize given message structure with message header to one buffer (attribute data).
     * Each message type has its own message structure. Type void* is chosen as universal type to represent it.
     * 
     * @param messageStruct pointer to specific message structure which will be serialized
     */
    virtual void serialize(void *messageStruct) = 0;

    /**
     * @brief Deserialize the message to given messageStruct. 
     * Each message type has its own message structure. Type void* is chosen as universal type to represent it.
     * 
     * @note In contrast with serialize() method, this method just deserialize the message payload without message header.
     * This is beacuse message header is deserialized during construction of the Message object.
     * 
     * @param messageStruct [out] pointer to specific message structure which will be filled with deserialized data
     */
    virtual void deserialize(void *messageStruct) const = 0;


    static std::string messageTypeToString(MessageType type){
        switch (type)
        {
        case MessageType::VERSION:
            return "VERSION";
        case MessageType::ACK:
            return "ACK";
        case MessageType::TX:
            return "TRANSACTION";
        case MessageType::BLOCK:
            return "BLOCK";
        case MessageType::ACCOUNT:
            return "ACCOUNT";
        case MessageType::INV:
            return "INVENTORY";
        case MessageType::GETDATA:
            return "GETDATA";
        case MessageType::PROPOSAL:
            return "PROPOSAL";
        default:
            return "UNKNOW";
        }
    }

protected:
    message_hdr_t msgHdr;   ///< message header
    byteBuffer data;        ///< byte buffer representing message data (at the begining of these buffer is also the header)
private:
    size_t currentMessageSize; ///< size of added message fragments in bytes
};


class VersionMessage : public Message{
public:

    struct version_msg_t{
        uint32_t version;
        NodeType nodeType;
        byte64_t peerID;
    };

    VersionMessage(size_t messageSize) : Message(constructMessageHeader(messageSize)) {}
    VersionMessage(message_hdr_t &messageHeader) : Message(messageHeader) {}

    static size_t getPayloadSize(){
        return sizeof(uint32_t) + // sizeof(version)
               sizeof(uint32_t) + // sizeof(nodeType)
               sizeof(byte64_t);  // sizeof(peerID)
    }

    ///@brief messageStruct is version_msg_t structure
    void serialize(void *messageStruct) override;

    /// @brief messageStruct is version_msg_t structure
    void deserialize(void *messageStruct) const override;


private:
    static message_hdr_t constructMessageHeader(size_t messageSize){
        message_hdr_t hdr;
        hdr.magicNum = MESSAGE_MAGIC_CONST;
        hdr.type = MessageType::VERSION;
        hdr.size = messageSize;
        hdr.checkSum = 0;
        return hdr;
    }
};


class AckMessage : public Message{
public:

    AckMessage(size_t messageSize) : Message(constructMessageHeader(messageSize)) {}
    AckMessage(message_hdr_t &messageHeader) : Message(messageHeader) {}

    static size_t getPayloadSize(){
        return 0; // ACK message has no payload
    }

    /// @brief ACK has no payload messageStruct may be anything, this parameter is ignored
    void serialize(void *messageStruct) override;

    /// @brief ACK has no payload messageStruct may be anything, this parameter is ignored
    void deserialize(void *messageStruct) const override;

private:
    static message_hdr_t constructMessageHeader(size_t messageSize){
        message_hdr_t hdr;
        hdr.magicNum = MESSAGE_MAGIC_CONST;
        hdr.type = MessageType::ACK;
        hdr.size = messageSize;
        hdr.checkSum = 0;
        return hdr;
    } 
};


class BlockMessage : public Message{
public:

    BlockMessage(size_t messageSize) : Message(constructMessageHeader(messageSize)) {}
    BlockMessage(message_hdr_t &messageHeader) : Message(messageHeader) {}

    /// @brief messageStruct is a PQB::Block object
    void serialize(void *messageStruct) override;

    /// @brief messageStruct is a PQB::Block object
    void deserialize(void *messageStruct) const override;

private:
    static message_hdr_t constructMessageHeader(size_t messageSize){
        message_hdr_t hdr;
        hdr.magicNum = MESSAGE_MAGIC_CONST;
        hdr.type = MessageType::BLOCK;
        hdr.size = messageSize;
        hdr.checkSum = 0;
        return hdr;
    } 
};


class TransactionMessage : public Message{
public:

    TransactionMessage(size_t messageSize) : Message(constructMessageHeader(messageSize)) {}
    TransactionMessage(message_hdr_t &messageHeader) : Message(messageHeader) {}

    /// @brief messageStruct is PQB::Transaction object
    void serialize(void *messageStruct) override;

    /// @brief messageStruct is PQB::Transaction object
    void deserialize(void *messageStruct) const override;

private:
    static message_hdr_t constructMessageHeader(size_t messageSize){
        message_hdr_t hdr;
        hdr.magicNum = MESSAGE_MAGIC_CONST;
        hdr.type = MessageType::TX;
        hdr.size = messageSize;
        hdr.checkSum = 0;
        return hdr;
    } 
};


class AccountMessage : public Message{
public:

    AccountMessage(size_t messageSize) : Message(constructMessageHeader(messageSize)) {}
    AccountMessage(message_hdr_t &messageHeader) : Message(messageHeader) {}

    /// @brief messageStruct is PQB::Account object
    void serialize(void *messageStruct) override;

    /// @brief messageStruct is PQB::Account object
    void deserialize(void *messageStruct) const override;

private:
    static message_hdr_t constructMessageHeader(size_t messageSize){
        message_hdr_t hdr;
        hdr.magicNum = MESSAGE_MAGIC_CONST;
        hdr.type = MessageType::ACCOUNT;
        hdr.size = messageSize;
        hdr.checkSum = 0;
        return hdr;
    }

};


class InvMessage : public Message{
public:

    InvMessage(size_t messageSize) : Message(constructMessageHeader(messageSize)) {}
    InvMessage(message_hdr_t &messageHeader) : Message(messageHeader) {}

    /// @brief Determine size of InvMessage
    /// @param numOfInv number of inventories
    /// @return Size of InvMessage
    static size_t getPayloadSize(size_t numOfInv){
        return (sizeof(InvType) + sizeof(byte64_t)) * numOfInv;
    }

    /// @brief messageStruct is a vector of inv_message_t strucutres! This is because InvMessage can include more inventories.
    void serialize(void *messageStruct) override;

    /// @brief messageStruct is a vector of inv_message_t strucutres! This is because InvMessage can include more inventories.
    void deserialize(void *messageStruct) const override;

private:
    static message_hdr_t constructMessageHeader(size_t messageSize){
        message_hdr_t hdr;
        hdr.magicNum = MESSAGE_MAGIC_CONST;
        hdr.type = MessageType::INV;
        hdr.size = messageSize;
        hdr.checkSum = 0;
        return hdr;
    }
};


class GetDataMessage : public Message{
public:

    GetDataMessage(size_t messageSize) : Message(constructMessageHeader(messageSize)) {}
    GetDataMessage(message_hdr_t &messageHeader) : Message(messageHeader) {}

    /// @brief Determine size of GetDataMessage
    /// @param numOfInv number of inventories
    /// @return Size of GetDataMessage
    static size_t getPayloadSize(size_t numOfInv){
        return (sizeof(InvType) + sizeof(byte64_t)) * numOfInv;
    }

    /// @brief messageStruct is a vector of inv_message_t strucutres! This is because InvMessage can include more inventories.
    void serialize(void *messageStruct) override;

    /// @brief messageStruct is a vector of inv_message_t strucutres! This is because InvMessage can include more inventories.
    void deserialize(void *messageStruct) const override;

private:
    static message_hdr_t constructMessageHeader(size_t messageSize){
        message_hdr_t hdr;
        hdr.magicNum = MESSAGE_MAGIC_CONST;
        hdr.type = MessageType::GETDATA;
        hdr.size = messageSize;
        hdr.checkSum = 0;
        return hdr;
    }
};


namespace MessageCreator{

    /// @brief Create a Message object based on given Message header
    Message* createMessage(Message::message_hdr_t &msgHeader);

} // namespace MessageCreator


} // namespace PQB

/* END OF FILE */