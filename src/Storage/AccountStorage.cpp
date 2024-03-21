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
#include "PQBconstatnts.hpp"


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
    leveldb::Status status = leveldb::DB::Open(databaseOptions, PQB::ACCOUNTS_DATABASE_PATH, &db);
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
        /// @todo make log -> status.ToString()
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
    if (!status.ok())
        return false;
    return true;
}

void AccountBalanceStorage::setBalancesByTxSet(std::set<TransactionPtr, TransactionPtrComparator> &txSet){
    std::unordered_map<std::string, AccountDifference> txDiffs = countAccountDifferencesByTxSet(txSet);
    leveldb::WriteBatch batch;
    leveldb::Status status;
    for (const auto &tx : txDiffs){
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

std::unordered_map<std::string, AccountBalanceStorage::AccountDifference> AccountBalanceStorage::countAccountDifferencesByTxSet(std::set<TransactionPtr, TransactionPtrComparator> &txSet){
    std::unordered_map<std::string, AccountDifference> txDiffs;
    for (const auto &tx : txSet){
        std::string senderHash = tx->senderWalletAddress.getHex();
        const auto senderIt = txDiffs.find(senderHash);
        if (senderIt != txDiffs.end()){
            senderIt->second.balanceDiff -= tx->cashAmount;
            senderIt->second.txSequence = (tx->sequenceNumber > senderIt->second.txSequence ? tx->sequenceNumber : senderIt->second.txSequence);
        } else {
            txDiffs[senderHash] = {.id=&tx->senderWalletAddress, .balanceDiff=(0-tx->cashAmount), .txSequence=tx->sequenceNumber};
        }
        std::string receiverHash = tx->receiverWalletAddress.getHex();
        const auto receiverIt = txDiffs.find(receiverHash);
        if (receiverIt != txDiffs.end()){
            receiverIt->second.balanceDiff += tx->cashAmount;
        } else {
            txDiffs[receiverHash] = {.id=&tx->receiverWalletAddress, .balanceDiff=tx->cashAmount, .txSequence=0};
        }
    }
    return txDiffs;
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
    leveldb::Status status = leveldb::DB::Open(databaseOptions, PQB::ADDRESS_DATABASE_PATH, &db);
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
        /// @todo make log
        // throw PQB::Exceptions::Storage(status.ToString());
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
    if (!status.ok())
        return false;
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