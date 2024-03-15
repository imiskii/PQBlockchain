
#include <iostream>
#include <string>
#include <memory>
#include "Transaction.hpp"
#include "Block.hpp"
#include "Blob.hpp"
#include "HashManager.hpp"
#include "MerkleRootCompute.hpp"
#include "PQBtypedefs.hpp"
#include "BlocksStorage.hpp"
#include "AccountStorage.hpp"


int main(int argc, char *argv[]){

    // ArgParser &a = ArgParser::GetInstance(argc, argv);
    // args_t parsedArgs = a.getArguments();

    PQB::Signer::GetInstance("falcon1024");

    PQB::BlocksStorage bStorage;
    bStorage.openDatabase();

    PQB::byteBuffer pk1;
    PQB::byteBuffer sk1;

    PQB::byteBuffer pk2;
    PQB::byteBuffer sk2;

    PQB::SignAlgorithmPtr sa = PQB::Signer::GetInstance();
    sa->genKeys(sk1, pk1);
    sa->genKeys(sk2, pk2);

    std::string addr1 = "abc";
    std::string addr2 = "cde";
    std::string genessis = "0";

    byte64_t hash1;
    byte64_t hash2;
    byte64_t hash3;

    PQB::HashMan::SHA512_hash(&hash1, (PQB::byte*) addr1.data(), addr1.size());
    PQB::HashMan::SHA512_hash(&hash2, (PQB::byte*) addr2.data(), addr2.size());
    PQB::HashMan::SHA512_hash(&hash3, (PQB::byte*) genessis.data(), genessis.size());

    PQB::AccountStorage accStorage;
    accStorage.openDatabases();

    PQB::AccountStorage::AccountData ad1;
    ad1.bd.balance = 10000;
    ad1.bd.txSequence = 0;
    ad1.bd.publicKey = pk1;
    ad1.addresses.push_back("127.0.0.1");
    ad1.addresses.push_back("127.0.0.2");
    ad1.addresses.push_back("127.0.0.3");
    ad1.addresses.push_back("127.0.0.4");
    ad1.addresses.push_back("127.0.0.5");
    ad1.addresses.push_back("127.0.0.6");
    ad1.addresses.push_back("127.0.0.7");
    ad1.addresses.push_back("127.0.0.8");
    ad1.addresses.push_back("127.0.0.9");
    ad1.addresses.push_back("127.0.0.10");
    ad1.addresses.push_back("127.0.0.11");
    PQB::AccountStorage::AccountData ad2;
    ad2.bd.balance = 0;
    ad2.bd.txSequence = 0;
    ad2.bd.publicKey = pk1;
    ad2.addresses.push_back("127.0.0.1");

    accStorage.setAccount(hash1, ad1);
    accStorage.setAccount(hash2, ad2);

    PQB::Block b;
    b.version = 1;
    b.previousBlockHash = hash3;

    PQB::TransactionPtr tx1 = std::make_shared<PQB::Transaction>();
    tx1->versionNumber = 1;
    tx1->senderWalletAddress = hash1;
    tx1->receiverWalletAddress = hash2;
    tx1->sequenceNumber = 1;
    tx1->timestamp = 42;
    tx1->cashAmount = 100;
    tx1->setHash();
    std::cout << "tx1 - " << "hash: " << tx1->IDHash.getHex() << std::endl;
    tx1->sign(sk1);
    b.addTransaction(tx1);

    PQB::TransactionPtr tx2 = std::make_shared<PQB::Transaction>();
    tx2->versionNumber = 1;
    tx2->senderWalletAddress = hash1;
    tx2->receiverWalletAddress = hash2;
    tx2->sequenceNumber = 2;
    tx2->timestamp = 43;
    tx2->cashAmount = 110;
    tx2->setHash();
    std::cout << "tx2 - " << "hash: " << tx2->IDHash.getHex() << std::endl;
    tx2->sign(sk1);
    b.addTransaction(tx2);

    PQB::TransactionPtr tx3 = std::make_shared<PQB::Transaction>();
    tx3->versionNumber = 1;
    tx3->senderWalletAddress = hash1;
    tx3->receiverWalletAddress = hash2;
    tx3->sequenceNumber = 3;
    tx3->timestamp = 44;
    tx3->cashAmount = 120;
    tx3->setHash();
    std::cout << "tx3 - " << "hash: " << tx3->IDHash.getHex() << std::endl;
    tx3->sign(sk1);
    b.addTransaction(tx3);

    b.size = b.getSize();
    b.timestamp = 45;
    b.transactionCount = 3;
    b.transactionsMerkleRootHash = PQB::ComputeBlocksMerkleRoot(b);
    accStorage.blncDB->setBalancesByTxSet(b.txSet);
    b.accountBalanceMerkleRootHash = accStorage.blncDB->getAccountsMerkleRootHash();

    byte64_t bHash = b.getBlockHash();

    PQB::byteBuffer buffer;
    buffer.resize(b.size);
    b.serialize(buffer);

    bStorage.setBlock(bHash, buffer);
    PQB::Block *bb = bStorage.getBlock(bHash);
    
    std::cout << "Block 1 - " << "size: " << b.size << std::endl;
    std::cout << "Block 1 - " << "tx count: " << b.transactionCount << std::endl;
    std::cout << "Block 1 - " << "tx hash: " << b.transactionsMerkleRootHash.getHex() << std::endl;
    std::cout << "Block 1 - " << "account hash: " << b.accountBalanceMerkleRootHash.getHex() << std::endl;
    std::cout << "Block 1 - " << "hash: " << bHash.getHex() << std::endl;

    byte64_t bbHash = bb->getBlockHash();

    std::cout << "Block 2 - " << "size: " << bb->size << std::endl;
    std::cout << "Block 2 - " << "tx count: " << bb->transactionCount << std::endl;
    std::cout << "Block 2 - " << "tx hash: " << bb->transactionsMerkleRootHash.getHex() << std::endl;
    std::cout << "Block 2 - " << "account hash: " << bb->accountBalanceMerkleRootHash.getHex() << std::endl;
    std::cout << "Block 2 - " << "hash: " << bbHash.getHex() << std::endl;

    accStorage.getAccount(hash1, ad1);
    accStorage.getAccount(hash2, ad2);

    std::cout << "Account 1 - " << "balance: " << ad1.bd.balance << std::endl;
    std::cout << "Account 1 - " << "seq. num.: " << ad1.bd.txSequence << std::endl;
    std::cout << "Account 1 - " << "Address: " << ad1.addresses.at(0) << std::endl << std::endl;

    std::cout << "Account 2 - " << "balance: " << ad2.bd.balance << std::endl;
    std::cout << "Account 2 - " << "seq. num.: " << ad2.bd.txSequence << std::endl;
    std::cout << "Account 2 - " << "Address: " << ad2.addresses.at(0) << std::endl << std::endl;

    for (const auto &addr : ad1.addresses){
        std::cout << "Account 1 - " << "Address: " << addr << std::endl;
    }
    
    std::cout << std::endl;

    for (const auto &tx : bb->txSet){
        if (tx->verify(pk1))
            std::cout <<  "Transcation " << tx->IDHash.getHex() << " is valid." << std::endl;
        else
            std::cout << "Transcation " << tx->IDHash.getHex() << " is not valid." << std::endl;
    }

    byte64_t accountHash = accStorage.blncDB->getAccountsMerkleRootHash();
    std::cout << std::endl << "Account hash for control " << accountHash.getHex() << std::endl;


    delete bb;

    return 0;
}

/* END OF FILE */