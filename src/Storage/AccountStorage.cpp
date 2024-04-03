/**
 * @file AccountStorage.cpp
 * @author Michal Ľaš
 * @brief LevelDB databese for storing account balances of blockchain's nodes
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "AccountStorage.hpp"
#include "PQBExceptions.hpp"
#include "PQBconstants.hpp"
#include "Log.hpp"

namespace PQB{


AccountBalanceStorage::AccountBalanceStorage(){
    db = nullptr;
    databaseOptions.create_if_missing = true;
}

AccountBalanceStorage::~AccountBalanceStorage(){
    if (db != nullptr)
        delete db;
}

void AccountBalanceStorage::Open(){
    leveldb::Status status = leveldb::DB::Open(databaseOptions, PQB::ACCOUNTS_DATABASE_PATH.data(), &db);
    if(!status.ok()){
        throw PQB::Exceptions::Storage(status.ToString());
    }
}

bool AccountBalanceStorage::getBalance(const byte64_t &walletID, AccountBalance &acc) const{
    std::string readValue;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), leveldb::Slice((char*)walletID.data(), walletID.size()), &readValue);
    if (status.IsNotFound()){
        return false;
    } else if (!status.ok()){
        PQB_LOG_ERROR("ACCOUNT STORAGE", "Failed to get account balance: {}", status.ToString());
        return false;
    }
    byteBuffer buffer(readValue.begin(), readValue.end());
    size_t offset = 0;
    acc.deserializeAccountBalance(buffer, offset);
    return true;
}

bool AccountBalanceStorage::setBalance(const byte64_t &walletID, AccountBalance &acc){
    byteBuffer buffer;
    buffer.resize(acc.getAccountBalanceSize());
    size_t offset = 0;
    acc.serializeAccountBalance(buffer, offset);
    leveldb::Slice value((char*) buffer.data(), buffer.size());
    leveldb::Status status = db->Put(leveldb::WriteOptions(), leveldb::Slice((char*)walletID.data(), walletID.size()), value);
    if (!status.ok()){
        PQB_LOG_ERROR("ACCOUNT STORAGE", "Failed to set account balance: {}", status.ToString());
        return false;
    }
    PQB_LOG_TRACE("ACCOUNT STORAGE", "Balance {} with seq. {} set for account: {}", acc.balance, acc.txSequence, shortStr(walletID.getHex()));
    return true;
}

void AccountBalanceStorage::setBalancesByAccDiffs(std::unordered_map<std::string, AccountDifference> &accDiffs){
    leveldb::WriteBatch batch;
    leveldb::Status status;
    for (const auto &tx : accDiffs){
        AccountBalance acc;
        if (!getBalance(*tx.second.id, acc)){
            std::string err = "Set Balances: wallet ID: " + tx.first + " is not in the database!"; 
            throw PQB::Exceptions::Storage(err);
        }

        acc.balance += tx.second.balanceDiff;
        // if txSequence is 0, it means that this this wallet was just receiving resources. So sequence number is not changing
        if (tx.second.txSequence != 0)
            acc.txSequence = tx.second.txSequence;

        byteBuffer buffer;
        buffer.resize(acc.getAccountBalanceSize());
        size_t offset = 0;

        acc.serializeAccountBalance(buffer, offset);
        leveldb::Slice value = leveldb::Slice((char*) buffer.data(), buffer.size());
        batch.Put(leveldb::Slice((char*)tx.second.id, tx.second.id->size()), value);
    }
    status = db->Write(leveldb::WriteOptions(), &batch);
    if (!status.ok())
        throw PQB::Exceptions::Storage(status.ToString());
    PQB_LOG_TRACE("ACCOUNT STORAGE", "Account balances updated by transaction set");
}

byte64_t AccountBalanceStorage::getAccountsMerkleRootHash(){
    leveldb::Iterator *it = db->NewIterator(leveldb::ReadOptions());
    std::vector<byte64_t> accountHashes;
    byte64_t hash;
    // Iterate the database and get account hashes to accountHashes vector
    for (it->SeekToFirst(); it->Valid(); it->Next()){
        HashMan::SHA512_hash(&hash, (PQB::byte*)it->value().data(), it->value().size());
        accountHashes.push_back(hash);
    }
    delete it;
    // Shrink the accountHashes vector and do Merke Root hash computation
    accountHashes.shrink_to_fit();
    return ComputeMerkleRoot(std::move(accountHashes));
}

/*** AccountAddressStorage ***/

AccountAddressStorage::AccountAddressStorage(){
    db = nullptr;
    databaseOptions.create_if_missing = true;
}

AccountAddressStorage::~AccountAddressStorage(){
    if (db != nullptr)
        delete db;
}

void AccountAddressStorage::Open(){
    leveldb::Status status = leveldb::DB::Open(databaseOptions, PQB::ADDRESS_DATABASE_PATH.data(), &db);
    if(!status.ok()){
        throw PQB::Exceptions::Storage(status.ToString());
    }
}

bool AccountAddressStorage::getAddresses(const byte64_t &walletID, AccountAddress &acc) const{
    std::string readValue;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), leveldb::Slice((char*)walletID.data(), walletID.size()), &readValue);
    if (status.IsNotFound()){
        return false;
    } else if (!status.ok()){
        PQB_LOG_ERROR("ACCOUNT STORAGE", "Failed to get account addresses: {}", status.ToString());
        return false;
    }
    byteBuffer buffer(readValue.begin(), readValue.end());
    size_t offset = 0;
    acc.deserializeAccountAddress(buffer, offset);
    return true;
}

bool AccountAddressStorage::setAddresses(const byte64_t &walletID, AccountAddress &acc){
    byteBuffer buffer;
    buffer.resize(acc.getAccountAddressSize());
    size_t offset = 0;
    acc.serializeAccountAddress(buffer, offset);
    leveldb::Slice value((char*) buffer.data(), buffer.size());
    leveldb::Status status = db->Put(leveldb::WriteOptions(), leveldb::Slice((char*)walletID.data(), walletID.size()), value);
    if (!status.ok()){
        PQB_LOG_ERROR("ACCOUNT STORAGE", "Failed to set account addresses: {}", status.ToString());
        return false;
    }
    PQB_LOG_TRACE("ACCOUNT STORAGE", "Addresses updated for account: {}", shortStr(walletID.getHex()));
    return true;
}

/*** AccountStorage ***/

AccountStorage::AccountStorage(){
    blncDB = new AccountBalanceStorage();
    addrDB = new AccountAddressStorage();
}

AccountStorage::~AccountStorage(){
    delete blncDB;
    delete addrDB;
}

void AccountStorage::openDatabases(){
    blncDB->Open();
    addrDB->Open();
}

bool AccountStorage::getAccount(const byte64_t &walletID, Account &acc){
    if (!blncDB->getBalance(walletID, acc))
        return false;
    if (!addrDB->getAddresses(walletID, acc))
        return false;
    return true;
}

bool AccountStorage::setAccount(const byte64_t &walletID, Account &acc){
    if (!blncDB->setBalance(walletID, acc))
        return false;
    if (!addrDB->setAddresses(walletID, acc))
        return false;
    return true;
}

} // namespace PQB

/* END OF FILE */