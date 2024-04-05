
#include <iostream>
#include <string>
#include <memory>
#include <unordered_map>
#include "Consensus.hpp"
#include "Chain.hpp"
#include "Transaction.hpp"
#include "Block.hpp"
#include "Blob.hpp"
#include "HashManager.hpp"
#include "MerkleRootCompute.hpp"
#include "PQBtypedefs.hpp"
#include "BlocksStorage.hpp"
#include "AccountStorage.hpp"
#include "Wallet.hpp"
#include "Log.hpp"


int main(int argc, char *argv[]){

    PQB::Log::init();

    // ArgParser &a = ArgParser::GetInstance(argc, argv);
    // args_t parsedArgs = a.getArguments();


    PQB::Signer::GetInstance("falcon1024");
    PQB::Wallet *wallet;
    std::string configFile = "tmp/conf.json";

    try{
        wallet = new PQB::Wallet(configFile);
    } catch (PQB::Exceptions::Wallet &err){
        std::cout << err.what() << std::endl;
        return 1;
    }

    if (!wallet->loadConfigurationFromFile()){
        std::cout << "Wallet failed to open!" << std::endl;
        return 1;
    }

    wallet->genNewKeys();


    PQB::BlocksStorage bStorage;
    bStorage.openDatabase();

    PQB::byteBuffer pk2;
    PQB::byteBuffer sk2;

    PQB::SignAlgorithmPtr sa = PQB::Signer::GetInstance();
    sa->genKeys(sk2, pk2);

    std::string addr1 = "abc";
    std::string addr2 = "cde";
    std::string genessis = "0";

    byte64_t hash2;
    byte64_t hash3;

    PQB::HashMan::SHA512_hash(&hash2, (PQB::byte*) addr2.data(), addr2.size());
    PQB::HashMan::SHA512_hash(&hash3, (PQB::byte*) genessis.data(), genessis.size());

    PQB::AccountStorage accStorage;
    accStorage.openDatabases();

    //PQB::AccountStorage::AccountData ad1;
    PQB::Account ad1;
    ad1.balance = 10000;
    ad1.txSequence = 0;
    ad1.publicKey = wallet->getPublicKey();
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

    PQB::Account ad2;
    ad2.balance = 0;
    ad2.txSequence = 0;
    ad2.publicKey = pk2;
    ad2.addresses.push_back("127.0.0.1");

    accStorage.setAccount(wallet->getWalletID(), ad1);
    accStorage.setAccount(hash2, ad2);

    PQB::Block b;
    b.version = 1;
    b.sequence = 1;
    b.previousBlockHash = hash3;

    std::string receiverHash = hash2.getHex();
    PQB::TransactionPtr tx;

    tx = wallet->createNewTransaction(receiverHash, 100);
    std::cout << "tx1 - " << "hash: " << tx->IDHash.getHex() << std::endl;
    b.addTransaction(tx);

    tx = wallet->createNewTransaction(receiverHash, 110);
    std::cout << "tx2 - " << "hash: " << tx->IDHash.getHex() << std::endl;
    b.addTransaction(tx);
    std::string txHash = tx->IDHash.getHex();

    tx = wallet->createNewTransaction(receiverHash, 120);
    std::cout << "tx3 - " << "hash: " << tx->IDHash.getHex() << std::endl;
    b.addTransaction(tx);

    b.size = b.getSize();
    b.transactionCount = 3;
    b.transactionsMerkleRootHash = PQB::ComputeBlocksMerkleRoot(b);
    
    std::unordered_map<std::string, PQB::AccountBalanceStorage::AccountDifference> accDiffs;
    PQB::ConsensusWrapper::countAccountDifferencesByTxSet(accStorage.blncDB, wallet, b.txSet, accDiffs);
    accStorage.blncDB->setBalancesByAccDiffs(accDiffs);
    b.accountBalanceMerkleRootHash = accStorage.blncDB->getAccountsMerkleRootHash();

    byte64_t bHash = b.getBlockHash();

    PQB::byteBuffer buffer;
    size_t offset = 0;
    buffer.resize(b.size);
    b.serialize(buffer, offset);

    bStorage.setBlock(bHash, buffer);
    
    std::cout << "Block 1 - " << "size: " << b.size << std::endl;
    std::cout << "Block 1 - " << "tx count: " << b.transactionCount << std::endl;
    std::cout << "Block 1 - " << "tx hash: " << b.transactionsMerkleRootHash.getHex() << std::endl;
    std::cout << "Block 1 - " << "account hash: " << b.accountBalanceMerkleRootHash.getHex() << std::endl;
    std::cout << "Block 1 - " << "hash: " << bHash.getHex() << std::endl;

    std::cout << std::endl;

    std::stringstream ss;
    bStorage.putBlockHeadersDataToStringStream(ss);
    std::cout << ss.str();
    ss.str("");
    ss.clear();

    std::cout << std::endl;

    accStorage.blncDB->putAccountDataToStringStream(ss);
    std::cout << ss.str();
    ss.str("");
    ss.clear();
    
    std::cout << std::endl;

    std::string bHashStr = bHash.getHex();
    bStorage.putBlockTxDataToStringStream(bHashStr, ss);
    std::cout << ss.str();
    ss.str("");
    ss.clear();

    std::cout << std::endl;

    byte64_t accountHash = accStorage.blncDB->getAccountsMerkleRootHash();
    std::cout << std::endl << "Account hash for control " << accountHash.getHex() << std::endl << std::endl;

    std::cout << std::endl;

    wallet->outputWalletTxRecords(ss);
    std::cout << ss.str() << std::endl;
    ss.str("");
    ss.clear();

    wallet->saveConfigurationToFile();
    delete wallet;

    // genesis block
    PQB::BlockPtr genesis = std::make_shared<PQB::Block>();
    genesis = PQB::Chain::getGenesisBlock();
    byte64_t genesisHash = genesis->getBlockHash();
    std::cout << "Genesis block hash:" << std::endl << genesisHash.getHex() << std::endl; 
    
    return 0;
}

/* END OF FILE */