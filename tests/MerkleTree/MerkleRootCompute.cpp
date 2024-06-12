
#include <gtest/gtest.h>
#include "MerkleRootCompute.hpp"


struct MerkleTreeTest : testing::Test{

    PQB::TransactionSet txSet;

    void SetUp() {
        PQB::TransactionPtr tx1 = std::make_shared<PQB::Transaction>();
        PQB::TransactionPtr tx2 = std::make_shared<PQB::Transaction>();
        PQB::TransactionPtr tx3 = std::make_shared<PQB::Transaction>();

        tx1->IDHash.setHex("21B4F4BD9E64ED355C3EB676A28EBEDAF6D8F17BDC365995B319097153044080516BD083BFCCE66121A3072646994C8430CC382B8DC543E84880183BF856CFF5");
        tx1->sequenceNumber = 1;
        tx2->IDHash.setHex("848B0779FF415F0AF4EA14DF9DD1D3C29AC41D836C7808896C4EBA19C51AC40A439CAF5E61EC88C307C7D619195229412EAA73FB2A5EA20D23CC86A9D8F86A0F");
        tx2->sequenceNumber = 2;
        tx3->IDHash.setHex("3D637AE63D59522DD3CB1B81C1AD67E56D46185B0971E0BC7DD2D8AD3B26090ACB634C252FC6A63B3766934314EA1A6E59FA0C8C2BC027A7B6A460B291CD4DFB");
        tx3->sequenceNumber = 3;

        txSet.insert(tx1);
        txSet.insert(tx2);
        txSet.insert(tx3);
    }

    void TearDown() { 

    }
};


TEST_F(MerkleTreeTest, Merkle_Root_Odd){
    byte64_t root = PQB::ComputeTxSetMerkleRoot(txSet);
    EXPECT_STREQ(
        root.getHex().c_str(),
        "BAFEB796899BB1DEDA596C0C146BBF7F8C4E04A0D3488D0F5D96B5531F5A317B51C04B0CA52467ACD4603EB105A0FC1447D453431A7BC7B17B40FC2B10459255"
    );
}


TEST_F(MerkleTreeTest, Merkle_Root_Even){
    PQB::TransactionPtr tx4 = std::make_shared<PQB::Transaction>();
    tx4->IDHash.setHex("2AC968752F624BE3E3DF46764B51B7831FEB70D40307DF5D587D4793BFFEAF8B4042A1FD6D465DF2AACC3304328D431EF10E083BAF690B8CC535480A4FEF092F");
    tx4->sequenceNumber = 4;
    txSet.insert(tx4);
    byte64_t root = PQB::ComputeTxSetMerkleRoot(txSet);
    EXPECT_STREQ(
        root.getHex().c_str(),
        "6594F932C421EE904B7D3C0A7BBC097DD23DBA201DC0E61D5848D5440FE458E59D895A93DF82130B18A54587450E9C69269798448B70186696176D3DDACE95C5"
    );

}

TEST_F(MerkleTreeTest, Merkle_Root_Tree_Only_One){
    PQB::TransactionSet txSetOne;
    PQB::TransactionPtr tx = std::make_shared<PQB::Transaction>();
    tx->IDHash.setHex("A");
    tx->sequenceNumber = 1;
    txSetOne.insert(tx);
    byte64_t root = PQB::ComputeTxSetMerkleRoot(txSetOne);
    EXPECT_STREQ(
        root.getHex().c_str(),
        "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
    );
}

TEST_F(MerkleTreeTest, Empty_Tree){
    PQB::TransactionSet txSetEmpty;
    txSetEmpty.clear();
    byte64_t root = PQB::ComputeTxSetMerkleRoot(txSetEmpty);
    EXPECT_STREQ(
        root.getHex().c_str(),
        "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E"
    );
}

