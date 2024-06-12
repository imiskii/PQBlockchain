
#include <gtest/gtest.h>
#include "Block.hpp"


struct BlockTest : testing::Test{

    PQB::Block block;
    PQB::Block empty_block;

    PQB::TransactionPtr tx1;
    PQB::TransactionPtr tx2;
    PQB::TransactionPtr tx3;

    void SetUp() { 
        tx1 = std::make_shared<PQB::Transaction>();
        tx2 = std::make_shared<PQB::Transaction>();
        tx3 = std::make_shared<PQB::Transaction>();

        tx1->IDHash.setHex("21B4F4BD9E64ED355C3EB676A28EBEDAF6D8F17BDC365995B319097153044080516BD083BFCCE66121A3072646994C8430CC382B8DC543E84880183BF856CFF5");
        tx1->sequenceNumber = 1;
        tx1->signature.resize(64, 'c');
        tx1->signatureSize = 64;
        tx1->senderWalletAddress.setHex("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E");
        tx1->receiverWalletAddress.setHex("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E");
        tx2->IDHash.setHex("848B0779FF415F0AF4EA14DF9DD1D3C29AC41D836C7808896C4EBA19C51AC40A439CAF5E61EC88C307C7D619195229412EAA73FB2A5EA20D23CC86A9D8F86A0F");
        tx2->sequenceNumber = 2;
        tx2->signature.resize(64, 'c');
        tx2->signatureSize = 64;
        tx2->senderWalletAddress.setHex("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E");
        tx2->receiverWalletAddress.setHex("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E");
        tx3->IDHash.setHex("3D637AE63D59522DD3CB1B81C1AD67E56D46185B0971E0BC7DD2D8AD3B26090ACB634C252FC6A63B3766934314EA1A6E59FA0C8C2BC027A7B6A460B291CD4DFB");
        tx3->sequenceNumber = 3;
        tx3->signature.resize(64, 'c');
        tx3->signatureSize = 64;
        tx3->senderWalletAddress.setHex("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E");
        tx3->receiverWalletAddress.setHex("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E");

        block.addTransaction(tx1);
        block.addTransaction(tx2);
        block.addTransaction(tx3);
        block.transactionCount = 3;

        block.previousBlockHash.setHex("3173F0564AB9462B0978A765C1283F96F05AC9E9F8361EE1006DC905C153D85BF0E4C45622E5E990ABCF48FB5192AD34722E8D6A723278B39FEF9E4F9FC62378");
        block.transactionsMerkleRootHash.setHex("4921DE1EDB2ECC8CA3A22823705194B902CFA471675F2D1AE8BF67D0C7B060A7C192E36FFCA9F1A0D90AC2DBBDAF429EE1EC97E160EB00DC80B07000935304F3");
        block.accountBalanceMerkleRootHash.setHex("3225DFF071CD0CCFF736B0B159CC722963310C008472BE814669451A062C25C5F7654F079D3E0AE1CF2FDA1551A5A0B1F5E988383BE7D383D57F73D4012C4024");
        block.sequence = 2;
        block.version = 1;

        empty_block.txSet.clear();
        empty_block.transactionCount = 0;
        empty_block.previousBlockHash.setHex("3173F0564AB9462B0978A765C1283F96F05AC9E9F8361EE1006DC905C153D85BF0E4C45622E5E990ABCF48FB5192AD34722E8D6A723278B39FEF9E4F9FC62378");
        empty_block.transactionsMerkleRootHash.setHex("4921DE1EDB2ECC8CA3A22823705194B902CFA471675F2D1AE8BF67D0C7B060A7C192E36FFCA9F1A0D90AC2DBBDAF429EE1EC97E160EB00DC80B07000935304F3");
        empty_block.accountBalanceMerkleRootHash.setHex("3225DFF071CD0CCFF736B0B159CC722963310C008472BE814669451A062C25C5F7654F079D3E0AE1CF2FDA1551A5A0B1F5E988383BE7D383D57F73D4012C4024");
        empty_block.sequence = 2;
        empty_block.version = 1;
    }

    void TearDown() { 

    }
};


TEST_F(BlockTest, Serialize_Deserialize){
    PQB::byteBuffer buffer;
    buffer.resize(block.getSize());
    size_t offset = 0;
    block.serialize(buffer, offset);
    PQB::Block b;
    offset = 0;
    b.deserialize(buffer, offset);
    EXPECT_STREQ(
        b.previousBlockHash.getHex().c_str(),
        "3173F0564AB9462B0978A765C1283F96F05AC9E9F8361EE1006DC905C153D85BF0E4C45622E5E990ABCF48FB5192AD34722E8D6A723278B39FEF9E4F9FC62378"
    );
    EXPECT_STREQ(
        b.transactionsMerkleRootHash.getHex().c_str(),
        "4921DE1EDB2ECC8CA3A22823705194B902CFA471675F2D1AE8BF67D0C7B060A7C192E36FFCA9F1A0D90AC2DBBDAF429EE1EC97E160EB00DC80B07000935304F3"
    );
    EXPECT_STREQ(
        b.accountBalanceMerkleRootHash.getHex().c_str(),
        "3225DFF071CD0CCFF736B0B159CC722963310C008472BE814669451A062C25C5F7654F079D3E0AE1CF2FDA1551A5A0B1F5E988383BE7D383D57F73D4012C4024"
    );
    EXPECT_EQ(b.version, 1);
    EXPECT_EQ(b.sequence, 2);
    EXPECT_EQ(b.transactionCount, 3);
    EXPECT_EQ(b.txSet.size(), 3);
    EXPECT_TRUE(b.txSet.find(tx1) != b.txSet.end());
    EXPECT_TRUE(b.txSet.find(tx2) != b.txSet.end());
    EXPECT_TRUE(b.txSet.find(tx3) != b.txSet.end());
}

TEST_F(BlockTest, Serialize_Deserialize_Empty){
    PQB::byteBuffer buffer;
    buffer.resize(empty_block.getSize());
    size_t offset = 0;
    empty_block.serialize(buffer, offset);
    PQB::Block b;
    offset = 0;
    b.deserialize(buffer, offset);
    EXPECT_STREQ(
        b.previousBlockHash.getHex().c_str(),
        "3173F0564AB9462B0978A765C1283F96F05AC9E9F8361EE1006DC905C153D85BF0E4C45622E5E990ABCF48FB5192AD34722E8D6A723278B39FEF9E4F9FC62378"
    );
    EXPECT_STREQ(
        b.transactionsMerkleRootHash.getHex().c_str(),
        "4921DE1EDB2ECC8CA3A22823705194B902CFA471675F2D1AE8BF67D0C7B060A7C192E36FFCA9F1A0D90AC2DBBDAF429EE1EC97E160EB00DC80B07000935304F3"
    );
    EXPECT_STREQ(
        b.accountBalanceMerkleRootHash.getHex().c_str(),
        "3225DFF071CD0CCFF736B0B159CC722963310C008472BE814669451A062C25C5F7654F079D3E0AE1CF2FDA1551A5A0B1F5E988383BE7D383D57F73D4012C4024"
    );
    EXPECT_EQ(b.version, 1);
    EXPECT_EQ(b.sequence, 2);
    EXPECT_EQ(b.transactionCount, 0);
    EXPECT_EQ(b.txSet.size(), 0);
}

TEST_F(BlockTest, Missing_Merkle_Root){
    block.transactionsMerkleRootHash.SetNull();
    PQB::byteBuffer buffer;
    buffer.resize(block.getSize());
    size_t offset;
    EXPECT_THROW(block.serialize(buffer, offset), PQB::Exceptions::Block);
}

TEST_F(BlockTest, Missing_Previous_Hash){
    block.previousBlockHash.SetNull();
    PQB::byteBuffer buffer;
    buffer.resize(block.getSize());
    size_t offset;
    EXPECT_THROW(block.serialize(buffer, offset), PQB::Exceptions::Block);
}

TEST_F(BlockTest, Missing_Account_Hash){
    block.accountBalanceMerkleRootHash.SetNull();
    PQB::byteBuffer buffer;
    buffer.resize(block.getSize());
    size_t offset;
    EXPECT_THROW(block.serialize(buffer, offset), PQB::Exceptions::Block);
}

TEST_F(BlockTest, Hash){
    byte64_t blockHash = block.getBlockHash();
    EXPECT_STREQ(
        blockHash.getHex().c_str(),
        "5788A5C113EBBA196D252F165F9AA08570FB9B31CAF8DCF32925F350DD70907A72F09B09802AE0926A13E49BD934B1AC1291FC12738504DCBF3A9DB60A1CE858"
    );
}