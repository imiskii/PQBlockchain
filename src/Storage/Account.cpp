/**
 * @file Account.cpp
 * @author Michal Ľaš
 * @brief Account object representing peer on a blockchain
 * @date 2024-03-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "Account.hpp"

namespace PQB{

    void AccountBalance::serializeAccountBalance(byteBuffer &buffer, size_t &offset){
        if ((buffer.size() - offset) < getAccountBalanceSize())
            throw PQB::Exceptions::Storage("Seralization: buffer is smaller than BalanceData structure! Can not serialize!");

        std::memcpy(buffer.data() + offset, publicKey.data(), publicKey.size());
        offset += publicKey.size();
        serializeField(buffer, offset, balance);
        serializeField(buffer, offset, txSequence);
    }

    void AccountBalance::deserializeAccountBalance(const byteBuffer &buffer, size_t &offset){
        if ((buffer.size() - offset) < getAccountBalanceSize())
            throw PQB::Exceptions::Storage("Deseralization: buffer is smaller than BalanceData structure! Can not deserialize!");

        publicKey.resize(Signer::GetInstance()->getPublicKeySize());
        std::memcpy(publicKey.data(), buffer.data() + offset, publicKey.size());
        offset += publicKey.size();
        deserializeField(buffer, offset, balance);
        deserializeField(buffer, offset, txSequence);
    }

    size_t AccountAddress::getAccountAddressSize() const{
        size_t size = sizeof(uint32_t);     /// number of addresses
        for (const auto &addr : addresses){
            size += (sizeof(uint8_t) + addr.size());
        }
        return size;
    }

    void AccountAddress::serializeAccountAddress(byteBuffer &buffer, size_t &offset){
        uint32_t nAddresses = (addresses.size() > MAX_ACCOUNT_ADDRESSES ? MAX_ACCOUNT_ADDRESSES : addresses.size());
        if ((buffer.size() - offset) < getAccountAddressSize())
            throw PQB::Exceptions::Storage("Seralization: buffer is smaller than addresses! Can not serialize!");
        serializeField(buffer, offset, nAddresses);
        uint8_t splitMark = '\0';
        size_t nSer = 0;
        while (nSer < MAX_ACCOUNT_ADDRESSES && nSer < addresses.size()){
            std::memcpy(buffer.data() + offset, addresses[nSer].data(), addresses[nSer].size());
            offset += addresses[nSer].size();
            serializeField(buffer, offset, splitMark);
            nSer++;
        }
    }

    void AccountAddress::deserializeAccountAddress(const byteBuffer &buffer, size_t &offset){
        uint32_t nAddresses;
        if ((buffer.size() - offset) < sizeof(nAddresses))
            throw PQB::Exceptions::Storage("Deseralization: buffer is too small to deserialize addresses!");
        deserializeField(buffer, offset, nAddresses);
        if (nAddresses > MAX_ACCOUNT_ADDRESSES){
            return;
        }
        for (size_t i = 0; i < nAddresses; i++){
            std::string address;
            while (buffer.size() >= offset){
                if (buffer[offset] == '\0'){
                    offset += 1;
                    break;
                }
                address.push_back(buffer[offset]);
                offset += 1;
            }
            addresses.push_back(address);
        }
    }

    byte64_t Account::getAccountID(){
        if (id.IsNull() && !publicKey.empty()){
            HashMan::SHA512_hash(&id, publicKey.data(), publicKey.size());
        }
        return id;
    }

    void Account::serialize(byteBuffer &buffer, size_t &offset)
    {
        serializeAccountBalance(buffer, offset);
        serializeAccountAddress(buffer, offset);
    }

    void Account::deserialize(const byteBuffer &buffer, size_t &offset){
        deserializeAccountBalance(buffer, offset);
        deserializeAccountAddress(buffer, offset);
    }

} // namespace PQB

/* END OF FILE */