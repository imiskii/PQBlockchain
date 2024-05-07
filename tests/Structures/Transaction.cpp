
#include <gtest/gtest.h>
#include "Transaction.hpp"


struct TransactionTest : testing::Test{

    PQB::Transaction tx;

    void SetUp() { 
        tx.IDHash.setHex("21B4F4BD9E64ED355C3EB676A28EBEDAF6D8F17BDC365995B319097153044080516BD083BFCCE66121A3072646994C8430CC382B8DC543E84880183BF856CFF5");
        tx.signature.resize(64, 'c');
        tx.signatureSize = 64;
        tx.senderWalletAddress.setHex("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E");
        tx.receiverWalletAddress.setHex("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E");
        tx.cashAmount = 42;
        tx.sequenceNumber = 11;
        tx.timestamp = 1000;
        tx.versionNumber = 1;
    }

    void TearDown() { 

    }
};


TEST_F(TransactionTest, Serialize_Deserialize){
    PQB::byteBuffer buffer;
    buffer.resize(tx.getSize());
    size_t offset = 0;
    tx.serialize(buffer, offset);
    PQB::Transaction tx_t;
    offset = 0;
    tx_t.deserialize(buffer, offset);

    EXPECT_STREQ(
        tx_t.IDHash.getHex().c_str(),
        "21B4F4BD9E64ED355C3EB676A28EBEDAF6D8F17BDC365995B319097153044080516BD083BFCCE66121A3072646994C8430CC382B8DC543E84880183BF856CFF5"
    );
    EXPECT_STREQ(
        tx_t.senderWalletAddress.getHex().c_str(),
        "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E"
    );
    EXPECT_STREQ(
        tx_t.receiverWalletAddress.getHex().c_str(),
        "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E"
    );
    EXPECT_EQ(tx_t.cashAmount, 42);
    EXPECT_EQ(tx_t.sequenceNumber, 11);
    EXPECT_EQ(tx_t.timestamp, 1000);
    EXPECT_EQ(tx_t.versionNumber, 1);
}

TEST_F(TransactionTest, Missing_Receiver){
    tx.receiverWalletAddress.SetNull();
    PQB::byteBuffer buffer;
    buffer.resize(tx.getSize());
    size_t offset = 0;
    EXPECT_THROW(tx.serialize(buffer, offset), PQB::Exceptions::Transaction);
}

TEST_F(TransactionTest, Missing_Sender){
    tx.senderWalletAddress.SetNull();
    PQB::byteBuffer buffer;
    buffer.resize(tx.getSize());
    size_t offset = 0;
    EXPECT_THROW(tx.serialize(buffer, offset), PQB::Exceptions::Transaction);
}

TEST_F(TransactionTest, Missing_Signature){
    tx.signature.clear();
    tx.signatureSize = 0;
    PQB::byteBuffer buffer;
    EXPECT_THROW(buffer.resize(tx.getSize()), PQB::Exceptions::Transaction);
}

TEST_F(TransactionTest, Missing_ID){
    tx.IDHash.SetNull();
    PQB::byteBuffer buffer;
    buffer.resize(tx.getSize());
    size_t offset = 0;
    EXPECT_THROW(tx.serialize(buffer, offset), PQB::Exceptions::Transaction);
}