
#include <gtest/gtest.h>
#include "Log.hpp"
#include "Signer.hpp"
#include "Account.hpp"
#include "BlocksStorage.hpp"


struct BlockStorageTest : testing::Test{

    PQB::BlocksStorage *blockS;
    PQB::Block block;
    byte64_t block_id;

    void SetUp() {
        PQB::Log::init(); // to avoid segfault from uninitialized logger
        auto ss = PQB::Signer::GetInstance("ed25519");

        block.txSet.clear();
        block.transactionCount = 0;
        block.previousBlockHash.setHex("3173F0564AB9462B0978A765C1283F96F05AC9E9F8361EE1006DC905C153D85BF0E4C45622E5E990ABCF48FB5192AD34722E8D6A723278B39FEF9E4F9FC62378");
        block.transactionsMerkleRootHash.setHex("4921DE1EDB2ECC8CA3A22823705194B902CFA471675F2D1AE8BF67D0C7B060A7C192E36FFCA9F1A0D90AC2DBBDAF429EE1EC97E160EB00DC80B07000935304F3");
        block.accountBalanceMerkleRootHash.setHex("3225DFF071CD0CCFF736B0B159CC722963310C008472BE814669451A062C25C5F7654F079D3E0AE1CF2FDA1551A5A0B1F5E988383BE7D383D57F73D4012C4024");
        block.sequence = 2;
        block.version = 1;
        block_id = block.getBlockHash();

        blockS = new PQB::BlocksStorage();
        blockS->openDatabase();
    }

    void TearDown() {
        delete blockS;
    }
};


TEST_F(BlockStorageTest, Set_Block){
    EXPECT_TRUE(blockS->setBlock(&block));

}

TEST_F(BlockStorageTest, Get_Block){
    PQB::Block *b = blockS->getBlock(block_id);
    ASSERT_TRUE(b != nullptr);
    EXPECT_EQ(b->transactionCount, 0);
    EXPECT_EQ(b->sequence, 2);
    EXPECT_EQ(b->version, 1);
    EXPECT_STREQ(
        b->previousBlockHash.getHex().c_str(),
        "3173F0564AB9462B0978A765C1283F96F05AC9E9F8361EE1006DC905C153D85BF0E4C45622E5E990ABCF48FB5192AD34722E8D6A723278B39FEF9E4F9FC62378"
    );
    EXPECT_STREQ(
        b->transactionsMerkleRootHash.getHex().c_str(),
        "4921DE1EDB2ECC8CA3A22823705194B902CFA471675F2D1AE8BF67D0C7B060A7C192E36FFCA9F1A0D90AC2DBBDAF429EE1EC97E160EB00DC80B07000935304F3"
    );
    EXPECT_STREQ(
        b->accountBalanceMerkleRootHash.getHex().c_str(),
        "3225DFF071CD0CCFF736B0B159CC722963310C008472BE814669451A062C25C5F7654F079D3E0AE1CF2FDA1551A5A0B1F5E988383BE7D383D57F73D4012C4024"
    );
}
