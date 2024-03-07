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

namespace PQB{


enum class MessageType : uint32_t{
    TX,
    INV,
    VERSION,

};


/// @brief first 32 bits of SHA-512 hash of empty string. This constant is used as check if message header was parsed successfuly
const uint32_t MESSAGE_MAGIC_CONST = 4178435447;

class Message{
public:

    struct message_hdr_t{
        uint32_t magicNum;  ///< Number for checking if header was well parsed
        MessageType type;   ///< Type of the message
        uint32_t size;      ///< Size of message payload (without header size !)
        uint32_t checkSum;  ///< Number for checking if message was well parsed
    };

    Message(const message_hdr_t &messageHeader);


    /// @brief get size of message data (with header) in bytes
    size_t getSize() const {
        return messageSize;
    }

    /// @brief get pointer to message data (with header)
    PQB::byte *getData(){
        return data.data();
    }

    /// @brief get type of the message (see enum class MessageType)
    MessageType getType() const {
        return msgHdr.type;
    }

    /// @brief get header of this message
    message_hdr_t getMessageHeader() const {
        return msgHdr;
    }

    /**
     * @brief Add part of the message at the end of current message. At start iterator is at the begining of data.
     * When reciving message fragments they should also include the header of the message, so it will be also included in data of the message.
     * 
     * @param fragment pointer to message data
     * @param size size of the data
     */
    void addFragment(const char* fragment, size_t size);


    bool checkMagicNum(){
        return msgHdr.magicNum == MESSAGE_MAGIC_CONST;
    }

    /// @brief check message by checkSum (message data has to be complete before checking else it returns false)
    bool checkMessage() const;

    /// @brief set the check sum of this message (check sum is 32 bits of message data hash)
    void setCheckSum();

protected:
    message_hdr_t msgHdr;   ///< message header
    size_t messageSize;     ///< size of the message (with header size)
    byteBuffer data;        ///< byte buffer representing message data (at the begining of these buffer is also the header)
    byteBuffer::iterator messageDataStart; ///< iterator to place where are message data starting at 'data' (place after message header)
private:
    size_t currentMessageSize; ///< size of added message fragments in bytes
};


/// @brief Shared pointer to message
typedef std::shared_ptr<Message> MessageShPtr;


class VersionMessage : public Message{
public:

    struct version_msg_t{
        uint32_t num;
    };

    VersionMessage() : Message(constructMessageHeader()) {}

    /// @brief return structure with message data
    version_msg_t getMessage() const;

    /// @brief set data of this function to provided data (if data are already set then override it)
    void setMessage(version_msg_t *message);

private:
    static message_hdr_t constructMessageHeader(){
        message_hdr_t hdr;
        hdr.checkSum = 0;
        hdr.size = sizeof(struct version_msg_t);
        hdr.type = MessageType::VERSION;
        return hdr;
    }
};

} // namespace PQB

/* END OF FILE */