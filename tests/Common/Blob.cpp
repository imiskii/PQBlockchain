
#include <gtest/gtest.h>
#include "Blob.hpp"

#include <string>
#include <cstring>

struct BlobTest : public testing::Test{
    byte64_t *blob;
    byte64_t *blob2;
    void SetUp() { 
        blob = new byte64_t(); 
        blob2 = new byte64_t(4);
    }

    void TearDown() { 
        delete blob;
        delete blob2;
    }
};


TEST_F(BlobTest, Is_Null){
    ASSERT_TRUE(blob->IsNull());
}

TEST_F(BlobTest, Set_Null){
    ASSERT_FALSE(blob2->IsNull());
    blob2->SetNull();
    ASSERT_TRUE(blob2->IsNull());
}

TEST_F(BlobTest, Set_Hex){
    std::string hex = "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E";
    blob->setHex(hex);
    blob2->setHex(hex);
    ASSERT_FALSE(blob->IsNull());
    ASSERT_FALSE(blob2->IsNull());
    ASSERT_EQ((std::memcmp(blob->data(), blob2->data(), blob->size())), 0);
}

TEST_F(BlobTest, GetHex){
    std::string hex = "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E";
    blob->setHex(hex);
    ASSERT_TRUE(blob->getHex() == hex);
}

TEST_F(BlobTest, Blob_EQ_1){
    byte64_t b(4);
    ASSERT_TRUE(*blob2 == b);
}

TEST_F(BlobTest, Blob_EQ_2){
    std::string hex1 = "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E";
    blob->setHex(hex1);
    blob2->setHex(hex1);
    ASSERT_TRUE(*blob == *blob2);
}

TEST_F(BlobTest, Blob_NEQ_1){
    ASSERT_TRUE(*blob != *blob2);
}

TEST_F(BlobTest, Blob_NEQ_2){
    std::string hex1 = "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E";
    std::string hex2 = "F90DDD77E400DFE6A3FCF479B00B1EE29E7015C5BB8CD70F5F15B4886CC339275FF553FC8A053F8DDC7324F45168CFFAF81F8C3AC93996F6536EEF38E5E40768";
    blob->setHex(hex1);
    blob2->setHex(hex2);
    ASSERT_TRUE(*blob != *blob2);
}

TEST_F(BlobTest, Blob_Greater_1){
    ASSERT_TRUE(*blob < *blob2);
}

TEST_F(BlobTest, Blob_Greater_2){
    std::string hex1 = "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E";
    std::string hex2 = "F90DDD77E400DFE6A3FCF479B00B1EE29E7015C5BB8CD70F5F15B4886CC339275FF553FC8A053F8DDC7324F45168CFFAF81F8C3AC93996F6536EEF38E5E40768";
    blob->setHex(hex1);
    blob2->setHex(hex2);
    ASSERT_TRUE(*blob < *blob2);
}