
#include <iostream>
#include <string>
#include <memory>
#include "Transaction.hpp"
#include "Block.hpp"
#include "Blob.hpp"
#include "HashManager.hpp"
#include "MerkleRootCompute.hpp"
#include "PQBtypedefs.hpp"


int main(int argc, char *argv[]){

    // ArgParser &a = ArgParser::GetInstance(argc, argv);
    // args_t parsedArgs = a.getArguments();

    PQB::byteBuffer pk;
    PQB::byteBuffer sk;

    PQB::SignAlgorithmPtr sa = PQB::Signer::GetInstance();
    sa->genKeys(sk, pk);

    std::string addr1 = "abc";
    std::string addr2 = "cde";
    std::string genessis = "0";

    byte64_t hash1;
    byte64_t hash2;
    byte64_t hash3;

    PQB::HashMan::SHA512_hash(&hash1, (PQB::byte*) addr1.data(), addr1.size());
    PQB::HashMan::SHA512_hash(&hash2, (PQB::byte*) addr2.data(), addr2.size());
    PQB::HashMan::SHA512_hash(&hash3, (PQB::byte*) genessis.data(), genessis.size());

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
    tx1->sign(sk);
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
    tx2->sign(sk);
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
    tx3->sign(sk);
    b.addTransaction(tx3);

    b.size = b.getSize();
    b.timestamp = 45;
    b.transactionCount = 3;
    b.transactionsMerkleRootHash = PQB::ComputeBlocksMerkleRoot(b);
    b.accountBalanceMerkleRootHash = hash3;

    byte64_t bHash = b.getBlockHash();

    PQB::byteBuffer buffer;
    buffer.resize(b.size);
    b.serialize(buffer);

    PQB::Block bb;
    bb.deserialize(buffer);
    
    std::cout << "Block 1 - " << "size: " << b.size << std::endl;
    std::cout << "Block 1 - " << "hash: " << b.transactionCount << std::endl;
    std::cout << "Block 1 - " << "tx hash: " << b.transactionsMerkleRootHash.getHex() << std::endl;
    std::cout << "Block 1 - " << "hash: " << bHash.getHex() << std::endl;

    byte64_t bbHash = b.getBlockHash();

    std::cout << "Block 2 - " << "size: " << bb.size << std::endl;
    std::cout << "Block 2 - " << "hash: " << bb.transactionCount << std::endl;
    std::cout << "Block 2 - " << "tx hash: " << bb.transactionsMerkleRootHash.getHex() << std::endl;
    std::cout << "Block 2 - " << "hash: " << bbHash.getHex() << std::endl;
    

    return 0;
}

/* END OF FILE */