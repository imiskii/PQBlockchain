/**
 * @file AddressStorage.cpp
 * @author Michal Ľaš
 * @brief LevelDB databese for storing nodes IPv4, IPv6 addresses and aliases
 * @date 2024-02-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "AddressStorage.hpp"
#include "PQBExceptions.hpp"
#include "PQBconstatnts.hpp"


namespace PQB{


AccountAddressStorage::AccountAddressStorage(){
    db = nullptr;
    databaseOptions.create_if_missing = true;
}

AccountAddressStorage::~AccountAddressStorage(){
    if(db != nullptr)
        delete db;
}

void AccountAddressStorage::openDatabase(){
    leveldb::Status status = leveldb::DB::Open(databaseOptions, PQB::ADDRESS_DATABASE_PATH, &db);
    if(!status.ok()){
        throw PQB::Exceptions::Storage(status.ToString());
    }
}

void AccountAddressStorage::getAddress(byte64_t &accountID)
{
}

void AccountAddressStorage::updateAddress(byte64_t &accountID, unsigned newAddress){

}


} // namespace PQB

/* END OF FILE */