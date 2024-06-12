
#include <gtest/gtest.h>
#include "Log.hpp"
#include "Signer.hpp"
#include "Wallet.hpp"
#include "Transaction.hpp"
#include "PQBtypedefs.hpp"


struct WalletTest : testing::Test{

    PQB::Wallet *wallet;
    byte64_t tx_id;

    void SetUp() {
        PQB::Log::init(); // to avoid segfault from uninitialized logger
        auto ss = PQB::Signer::GetInstance("ed25519");

        tx_id.setHex("CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E");

        std::string conf_file = std::string(PQB::CONFIGURATION_FILE);
        wallet = new PQB::Wallet(conf_file);
        ASSERT_TRUE(wallet->loadConfigurationFromFile());
        wallet->setBalance(10000);
    }

    void TearDown() {
        wallet->saveConfigurationToFile();
        delete wallet;
    }
};


TEST_F(WalletTest, Generate_Key_Pair){
    PQB::byteBuffer oldKey = wallet->getPublicKey();
    wallet->genNewKeys();
    EXPECT_TRUE(std::memcmp(oldKey.data(), wallet->getPublicKey().data(), oldKey.size()) != 0);
}

TEST_F(WalletTest, Create_Transaction){
    std::string receiver = "3173F0564AB9462B0978A765C1283F96F05AC9E9F8361EE1006DC905C153D85BF0E4C45622E5E990ABCF48FB5192AD34722E8D6A723278B39FEF9E4F9FC62378";
    PQB::TransactionPtr tx = wallet->createNewTransaction(receiver, 10);
    ASSERT_TRUE(tx != nullptr);
    EXPECT_EQ(wallet->getBalance(), 9990);
    EXPECT_EQ(tx->cashAmount, 10);
    EXPECT_EQ(tx->versionNumber, PQB::TX_VERSION);
    EXPECT_STREQ(
        tx->senderWalletAddress.getHex().c_str(),
        wallet->getWalletID().getHex().c_str()
    );
    EXPECT_STREQ(
        tx->receiverWalletAddress.getHex().c_str(),
        "3173F0564AB9462B0978A765C1283F96F05AC9E9F8361EE1006DC905C153D85BF0E4C45622E5E990ABCF48FB5192AD34722E8D6A723278B39FEF9E4F9FC62378"
    );
    EXPECT_TRUE(tx->verify(wallet->getPublicKey()));
}