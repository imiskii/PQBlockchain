/**
 * @file Account.hpp
 * @author Michal Ľaš
 * @brief Account object representing peer on a blockchain
 * @date 2024-03-19
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#pragma once

#include <string>
#include <vector>
#include "PQBtypedefs.hpp"
#include "PQBExceptions.hpp"
#include "Blob.hpp"
#include "Serialize.hpp"
#include "Signer.hpp"
#include "HashManager.hpp"


namespace PQB{


class AccountBalance{
public:
    byteBuffer publicKey;
    PQB::cash balance;
    uint32_t txSequence;

    AccountBalance(){
        setNull();
    }

    void setNull(){
        publicKey.clear();
        balance = 0;
        txSequence = 0;
    }

    size_t getAccountBalanceSize() const{
        return sizeof(balance) + sizeof(txSequence) + Signer::GetInstance()->getPublicKeySize();
    }

    void serializeAccountBalance(byteBuffer &buffer, size_t &offset);

    void deserializeAccountBalance(const byteBuffer &buffer, size_t &offset);

};


class AccountAddress{
public:

    /// @brief Maximum number of network addresses that can be stored for one account
    static const size_t MAX_ACCOUNT_ADDRESSES = 10;

    std::vector<std::string> addresses;

    AccountAddress(){
        setNull();
    }

    void setNull(){
        addresses.clear();
    }

    size_t getAccountAddressSize() const;

    void serializeAccountAddress(byteBuffer &buffer, size_t &offset);

    void deserializeAccountAddress(const byteBuffer &buffer, size_t &offset);

};


class Account : public AccountBalance, public AccountAddress{
public:

    Account(){
        setNull();
    }

    void setNull(){
        AccountBalance::setNull();
        AccountAddress::setNull();
        id.SetNull();
    }

    byte64_t getAccountID();

    size_t getSize() const{
        return getAccountBalanceSize() + getAccountAddressSize();
    }

    void serialize(byteBuffer &buffer, size_t &offset);

    void deserialize(const byteBuffer &buffer, size_t &offset);

private:
    byte64_t id;
};

} // namespace PQB

/* END OF FILE */