
#include <gtest/gtest.h>
#include "Log.hpp"
#include "Signer.hpp"
#include "Account.hpp"
#include "AccountStorage.hpp"


struct AccountStorageTest : testing::Test{

    PQB::AccountStorage *accS;
    PQB::Account acc;
    byte64_t acc_id;

    void SetUp() {
        PQB::Log::init(); // to avoid segfault from uninitialized logger
        auto ss = PQB::Signer::GetInstance("ed25519");

        acc.balance = 42;
        acc.txSequence = 11;
        acc.publicKey.resize(32, 'c');
        acc.addresses.push_back("127.0.0.1");
        acc_id = acc.getAccountID();

        accS = new PQB::AccountStorage();
        accS->openDatabases();
    }

    void TearDown() {
        delete accS;
    }
};


TEST_F(AccountStorageTest, Set_Balance){
    EXPECT_TRUE(accS->blncDB->setBalance(acc_id, acc));

}

TEST_F(AccountStorageTest, Get_Balance){
    PQB::AccountBalance a;
    ASSERT_TRUE(accS->blncDB->getBalance(acc_id, a));
    EXPECT_EQ(a.balance, acc.balance);
    EXPECT_EQ(a.txSequence, acc.txSequence);
    
}

TEST_F(AccountStorageTest, Set_Address){
    EXPECT_TRUE(accS->addrDB->setAddresses(acc_id, acc));
}

TEST_F(AccountStorageTest, Get_Address){
    PQB::AccountAddress a;
    ASSERT_TRUE(accS->addrDB->getAddresses(acc_id, a));
    EXPECT_STREQ(a.addresses[0].c_str(), acc.addresses[0].c_str());
}
