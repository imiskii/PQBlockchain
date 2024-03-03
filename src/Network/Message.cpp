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
#include "HashManager.hpp"


namespace PQB{

    Message::Message(const message_hdr_t &messageHeader){
        msgHdr = messageHeader;
        msgHdr.magicNum = MESSAGE_MAGIC_CONST;
        currentMessageSize = 0;
        data.reserve(getSize());
        data.clear();
        messageDataStart = (data.begin() + sizeof(struct message_hdr_t));
    }

    void Message::addFragment(const char *fragment, size_t size)
    {
        if ((currentMessageSize + size) <= msgHdr.size){
            data.insert(data.end(), fragment, fragment + size);
            currentMessageSize += size;
        }
    }

    bool Message::checkMessage() const{
        if (msgHdr.size == currentMessageSize){
            byte64_t messageHash;
            HashMan::SHA512_hash(&messageHash, data.data(), currentMessageSize);
            // compare 32 bits of hash with check sum
            return (!std::memcmp(messageHash.data(), &msgHdr.checkSum, sizeof(msgHdr.checkSum)));
        }
        return false;
    }

    void Message::setCheckSum(){
        byte64_t messageHash;
        HashMan::SHA512_hash(&messageHash, data.data(), msgHdr.size);
        std::memcpy(&msgHdr.checkSum, messageHash.data(), sizeof(msgHdr.checkSum));
        data.insert(data.begin(), &msgHdr, &msgHdr + sizeof(struct message_hdr_t));
    }


    VersionMessage::version_msg_t VersionMessage::getMessage() const {
        version_msg_t msg;
        std::memcpy(&msg, data.data() + sizeof(struct message_hdr_t), msgHdr.size);
        return msg;
    }

    void VersionMessage::setMessage(version_msg_t *message){
        data.insert(messageDataStart, message, message + msgHdr.size);
    }

} // namespace PQB

/* END OF FILE */