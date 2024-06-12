
#include <gtest/gtest.h>
#include "Proposal.hpp"

struct TxProposalTest : testing::Test{

    PQB::TxSetProposal *txProposal;

    void SetUp() { 
        txProposal = new PQB::TxSetProposal();
    }

    void TearDown() { 
        delete txProposal;
    }
};

struct BlockProposalTest : testing::Test{

    PQB::BlockProposal *blockProposal;

    void SetUp() { 
        blockProposal = new PQB::BlockProposal();
    }

    void TearDown() { 
        delete blockProposal;
    }
};


TEST_F(TxProposalTest, Hash){
    byte64_t hash1;
    byte64_t hash2;
    PQB::TxSetProposal p;
    txProposal->getHash(hash1);
    p.getHash(hash2);
    ASSERT_TRUE(hash1 == hash2);
    p.seq = 1;
    p.getHash(hash2);
    ASSERT_TRUE(hash1 != hash2);
}

TEST_F(TxProposalTest, Serialize_without_Signature){
    PQB::byteBuffer buffer;
    size_t offset = 0;
    EXPECT_THROW(txProposal->serialize(buffer, offset), PQB::Exceptions::Proposal);
}

TEST_F(TxProposalTest, Serialize_Deserialize_Empty){
    txProposal->seq = 42;
    txProposal->signature.resize(64, 'c');
    txProposal->signatureSize = 64;

    PQB::byteBuffer buffer;
    size_t offset = 0;
    buffer.resize(txProposal->getSize());
    txProposal->serialize(buffer, offset);

    offset = 0;
    PQB::TxSetProposal pp;
    pp.deserialize(buffer, offset);
    EXPECT_EQ(txProposal->seq, pp.seq);
}

TEST_F(TxProposalTest, Serialize_Deserialize){
    PQB::TransactionPtr tx = std::make_shared<PQB::Transaction>();
    tx->receiverWalletAddress.setHex("ABC");
    tx->senderWalletAddress.setHex("CAB");
    tx->signature.resize(64, 'c');
    tx->signatureSize = 64;
    tx->sequenceNumber = 11;
    tx->setHash();
    txProposal->seq = 42;
    txProposal->txSet.addTransaction(tx);
    txProposal->signature.resize(64, 'c');
    txProposal->signatureSize = 64;
    
    PQB::byteBuffer buffer;
    size_t offset = 0;
    buffer.resize(txProposal->getSize());
    txProposal->serialize(buffer, offset);

    offset = 0;
    PQB::TxSetProposal pp;
    pp.deserialize(buffer, offset);
    EXPECT_EQ(txProposal->seq, pp.seq);

    auto tx_t = txProposal->txSet.txSet.find(tx);
    EXPECT_TRUE(tx->IDHash == (*tx_t)->IDHash);
    EXPECT_TRUE(tx->senderWalletAddress == (*tx_t)->senderWalletAddress);
    EXPECT_TRUE(tx->receiverWalletAddress == (*tx_t)->receiverWalletAddress);
}

TEST_F(TxProposalTest, Serialize_to_Small_Buffer){
    txProposal->seq = 42;
    txProposal->signature.resize(64, 'c');
    txProposal->signatureSize = 64;

    PQB::byteBuffer buffer;
    size_t offset = 0;
    EXPECT_THROW(txProposal->serialize(buffer, offset), PQB::Exceptions::Proposal);
}

TEST_F(TxProposalTest, Deserialize_from_Small_Buffer){
    txProposal->seq = 42;
    txProposal->signature.resize(64, 'c');
    txProposal->signatureSize = 64;

    PQB::byteBuffer buffer;
    size_t offset = 0;
    EXPECT_THROW(txProposal->deserialize(buffer, offset), PQB::Exceptions::Proposal);
}