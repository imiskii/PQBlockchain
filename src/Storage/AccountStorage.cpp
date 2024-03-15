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

bool AccountBalanceStorage::getBalance(byte64_t &walletID, BalanceData &bd) const{
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
    bd = deserializeBalanceData(buffer, offset);
    return true;
}

bool AccountBalanceStorage::setBalance(byte64_t &walletID, BalanceData &bd){
    byteBuffer buffer;
    buffer.resize(getSizeOfBalanceData());
    size_t offset = 0;
    serializeBalanceData(buffer, offset, bd);
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
        BalanceData blnc;
        if (!getBalance(*tx.second.id, blnc)){
            std::string err = "Set Balances: wallet ID: " + tx.first + " is not in the database!"; 
            throw PQB::Exceptions::Storage(err);
        }

        blnc.balance += tx.second.balanceDiff;
        // if txSequence is 0, it means that this this wallet was just receiving resources. So sequence number is not changing
        if (tx.second.txSequence != 0)
            blnc.txSequence = tx.second.txSequence;

        byteBuffer buffer;
        buffer.resize(getSizeOfBalanceData());
        size_t offset = 0;

        serializeBalanceData(buffer, offset, blnc);
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

void AccountBalanceStorage::serializeBalanceData(byteBuffer &buffer, size_t &offset, BalanceData &bd){
    if ((buffer.size() - offset) < getSizeOfBalanceData())
        throw PQB::Exceptions::Storage("Seralization: buffer is smaller than BalanceData structure! Can not serialize!");

    std::memcpy(buffer.data() + offset, bd.publicKey.data(), bd.publicKey.size());
    offset += bd.publicKey.size();
    serializeField(buffer, offset, bd.balance);
    serializeField(buffer, offset, bd.txSequence);
}

AccountBalanceStorage::BalanceData AccountBalanceStorage::deserializeBalanceData(byteBuffer &buffer, size_t &offset){
    if ((buffer.size() - offset) < getSizeOfBalanceData())
        throw PQB::Exceptions::Storage("Deseralization: buffer is smaller than BalanceData structure! Can not deserialize!");

    BalanceData bd;
    bd.publicKey.resize(Signer::GetInstance()->getPublicKeySize());
    std::memcpy(bd.publicKey.data(), buffer.data() + offset, bd.publicKey.size());
    offset += bd.publicKey.size();
    deserializeField(buffer, offset, bd.balance);
    deserializeField(buffer, offset, bd.txSequence);
    return bd;
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

bool AccountAddressStorage::getAddresses(byte64_t &walletID, std::vector<std::string> &addresses) const{
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
    addresses = deserializeAddresses(buffer, offset);
    return true;
}

bool AccountAddressStorage::setAddresses(byte64_t &walletID, std::vector<std::string> &addresses){
    byteBuffer buffer;
    buffer.resize(getAddressesSize(addresses));
    size_t offset = 0;
    serializeAddresses(buffer, offset, addresses);
    leveldb::Slice value((char*) buffer.data(), buffer.size());
    leveldb::Status status = db->Put(leveldb::WriteOptions(), leveldb::Slice((char*)walletID.data(), walletID.size()), value);
    if (!status.ok())
        return false;
    return true;
}

size_t AccountAddressStorage::getAddressesSize(std::vector<std::string> &addresses){
    size_t size = sizeof(uint32_t);
    for (const auto &addr : addresses){
        size += (sizeof(uint8_t) + addr.size());
    }
    return size;
}

void AccountAddressStorage::serializeAddresses(byteBuffer &buffer, size_t &offset, std::vector<std::string> &addresses){
    uint32_t nAddresses = (addresses.size() > MAX_ACCOUNT_ADDRESSES ? MAX_ACCOUNT_ADDRESSES : addresses.size());
    if ((buffer.size() - offset) < getAddressesSize(addresses))
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

std::vector<std::string> AccountAddressStorage::deserializeAddresses(byteBuffer &buffer, size_t &offset){
    std::vector<std::string> addresses;
    uint32_t nAddresses;
    if ((buffer.size() - offset) < sizeof(nAddresses))
        throw PQB::Exceptions::Storage("Deseralization: buffer is too small to deserialize addresses!");
    deserializeField(buffer, offset, nAddresses);
    if (nAddresses > MAX_ACCOUNT_ADDRESSES){
        return addresses;
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
    return addresses;
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

bool AccountStorage::getAccount(byte64_t &walletID, AccountData &ad){
    if (!blncDB->getBalance(walletID, ad.bd))
        return false;
    if (!addrDB->getAddresses(walletID, ad.addresses))
        return false;
    return true;
}

bool AccountStorage::setAccount(byte64_t &walletID, AccountData &ad){
    if (!blncDB->setBalance(walletID, ad.bd))
        return false;
    if (!addrDB->setAddresses(walletID, ad.addresses))
        return false;
    return true;
}

void AccountStorage::serializeAccount(byteBuffer &buffer, AccountData &ad){
    size_t offset = 0;
    buffer.resize(blncDB->getSizeOfBalanceData() + addrDB->getAddressesSize(ad.addresses));
    blncDB->serializeBalanceData(buffer, offset, ad.bd);
    addrDB->serializeAddresses(buffer, offset, ad.addresses);
}

AccountStorage::AccountData AccountStorage::deserializeAccount(byteBuffer &buffer){
    AccountData ad;
    size_t offset = 0;
    ad.bd = blncDB->deserializeBalanceData(buffer, offset);
    ad.addresses = addrDB->deserializeAddresses(buffer, offset);
    return ad;
}

} // namespace PQB

/* END OF FILE */