
#include <gtest/gtest.h>
#include "Log.hpp"
#include "Account.hpp"
#include "Signer.hpp"


struct AccountTest : testing::Test{

    PQB::Account acc;

    void SetUp() {
        PQB::Log::init(); // to avoid segfault from uninitialized logger
        auto ss = PQB::Signer::GetInstance("ed25519");

        acc.balance = 42;
        acc.txSequence = 11;
        acc.publicKey.resize(32, 'c');
        acc.addresses.push_back("127.0.0.1");
        acc.addresses.push_back("127.0.0.2");
        acc.addresses.push_back("127.0.0.3");
        acc.addresses.push_back("127.0.0.4");
        acc.addresses.push_back("127.0.0.5");
        acc.addresses.push_back("127.0.0.6");
        acc.addresses.push_back("127.0.0.7");
        acc.addresses.push_back("127.0.0.8");
        acc.addresses.push_back("127.0.0.9");
        acc.addresses.push_back("127.0.0.10");
        acc.addresses.push_back("127.0.0.11");
    }

    void TearDown() { 

    }
};


TEST_F(AccountTest, Serialize_Deserialize){
    PQB::byteBuffer buffer;
    buffer.resize(acc.getSize());
    size_t offset = 0;

    acc.serialize(buffer, offset);
    PQB::Account a;
    offset = 0;
    a.deserialize(buffer, offset);

    EXPECT_EQ(acc.balance, a.balance);
    EXPECT_EQ(acc.txSequence, a.txSequence);
    EXPECT_EQ(a.addresses.size(), 10);
}

TEST_F(AccountTest, Hash){
    byte64_t id = acc.getAccountID();
    EXPECT_STREQ(
        id.getHex().c_str(),
        "7308948D99219C5D1DCA24F1FC07AAD940F821B53F17876AF3A12EC12A7463DC4234154CB281437A319EDB53220F110D8D51C5B716092BDFB4853D4809404AC3"
    );
}