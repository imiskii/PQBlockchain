/**
 * @file Database.cpp
 * @author Michal Ľaš
 * @brief 
 * @date 2024-02-20
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "Database.hpp"
#include "PQBExceptions.hpp"


namespace PQB{

    Database::Database(){
        db = nullptr;
        databaseInitializeOptions.create_if_missing = true;
        //databaseInitializeOptions.IncreaseParallelism();
        //databaseInitializeOptions.OptimizeLevelStyleCompaction();
    }

    Database::~Database(){
        closeDatabase();
    }

    void Database::openDatabase(){
        if (db != nullptr){
            leveldb::Status status = leveldb::DB::Open(databaseInitializeOptions, databasePath, &db);
            //if(!status.ok())
                //throw PQB::Exceptions::Storage(status.getState());
        }
    }

    void Database::closeDatabase(){
        delete db;
        db = nullptr;
    }


    cash AccountBalancesStorage::getAccountBalance(byte64_t &accountID){
        /*
        std::string readValue;
        rocksdb::Status status = db->Get(rocksdb::ReadOptions(), CFHandler, rocksdb::Slice((char*)accountID.data(), accountID.size()), &readValue);
        if (!status.ok())
            throw PQB::Exceptions::Storage(status.getState());
        cash value;
        std::memcpy(&value, readValue.data(), sizeof(cash));
        return value;
        */
    }

    void AccountBalancesStorage::updateBalance(byte64_t &accountID, cash newAmount){
        /*
        rocksdb::Slice value((char*) &newAmount, sizeof(cash));
        rocksdb::Status status = db->Put(rocksdb::WriteOptions(), CFHandler, rocksdb::Slice((char*)accountID.data(), accountID.size()), value);
        if (!status.ok())
            throw PQB::Exceptions::Storage(status.getState());
        */
    }

    void AccountBalancesStorage::updateBalancesByBlock(Block &block){
        /// @TODO: if will be from consensus set of transactons not from a block
        // Vytvor batch
        // preiteruj transakcie
        // odčítaj od source amoun, pričítaj k destination amount
        // write batch
    }


    Block *BlocksStorage::getBlock(size_t blockHeight){
        return nullptr;
    }

    void BlocksStorage::saveBlock(Block &block){

    }


    void AccountAddressStorage::getAddress(byte64_t &accountID){
        
    }

    void AccountAddressStorage::updateAddress(byte64_t &accountID, unsigned newAddress){

    }

} // namespace PQB

/* END OF FILE */