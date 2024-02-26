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


AccountBalancesStorage::AccountBalancesStorage(){
    db = nullptr;
    databaseOptions.create_if_missing = true;
}

AccountBalancesStorage::~AccountBalancesStorage(){
    if(db != nullptr)
        delete db;
}

void AccountBalancesStorage::openDatabase(){
    leveldb::Status status = leveldb::DB::Open(databaseOptions, PQB::ACCOUNTS_DATABASE_PATH, &db);
    if(!status.ok()){
        throw PQB::Exceptions::Storage(status.ToString());
    }
}

cash AccountBalancesStorage::getAccountBalance(byte64_t &accountID)
{
    std::string readValue;
    leveldb::Status status = db->Get(leveldb::ReadOptions(), leveldb::Slice((char*)accountID.begin(), accountID.size()), &readValue);
    if (!status.ok())
        throw PQB::Exceptions::Storage(status.ToString());
    cash value;
    std::memcpy(&value, readValue.data(), sizeof(cash));
    return value;
}

void AccountBalancesStorage::updateBalance(byte64_t &accountID, cash newAmount){
    leveldb::Slice value((char*) &newAmount, sizeof(cash));
    leveldb::Status status = db->Put(leveldb::WriteOptions(), leveldb::Slice((char*)accountID.data(), accountID.size()), value);
    if (!status.ok())
        throw PQB::Exceptions::Storage(status.ToString());
}

void AccountBalancesStorage::updateBalancesByBlock(unsigned &block){
    /// @TODO: if will be from consensus set of transactons not from a block
    // Vytvor batch
    // preiteruj transakcie
    // odčítaj od source amoun, pričítaj k destination amount
    // write batch
}


} // namespace PQB


/* END OF FILE */