
#include <gtest/gtest.h>
#include "HashManager.hpp"


TEST(HashManagerTest, Hash_Empty_String){
    byte64_t result;
    std::string str;
    str.clear();
    PQB::HashMan::SHA512_hash(&result, (PQB::byte *) str.data(), str.size());
    EXPECT_STREQ(
        result.getHex().c_str(),
        "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E"
    );
}

TEST(HashManagerTest, Hash_Byte_Array){
    byte64_t result;
    std::string str = "Hello world!";
    PQB::byteBuffer buffer;
    buffer.resize(str.size());
    std::memcpy(buffer.data(), str.data(), str.size());
    PQB::HashMan::SHA512_hash(&result, buffer.data(), buffer.size());
    EXPECT_STREQ(
        result.getHex().c_str(),
        "F6CDE2A0F819314CDDE55FC227D8D7DAE3D28CC556222A0A8AD66D91CCAD4AAD6094F517A2182360C9AACF6A3DC323162CB6FD8CDFFEDB0FE038F55E85FFB5B6"
    );
}

TEST(HashManagerTest, Hash_Hashes){
    byte64_t result;
    byte64_t first;
    byte64_t second;
    first.setHex("21B4F4BD9E64ED355C3EB676A28EBEDAF6D8F17BDC365995B319097153044080516BD083BFCCE66121A3072646994C8430CC382B8DC543E84880183BF856CFF5");
    second.setHex("848B0779FF415F0AF4EA14DF9DD1D3C29AC41D836C7808896C4EBA19C51AC40A439CAF5E61EC88C307C7D619195229412EAA73FB2A5EA20D23CC86A9D8F86A0F");
    PQB::HashMan::SHA512_hash(&result, first, second);
    EXPECT_STREQ(
        result.getHex().c_str(),
        "BA3E96C25D79453CD9F0E44EAECD8039F52AECB2BE8846D125C570374E5117713E364999653ECCAF2EED3B4A1B447AE59D294406DCC83F44183328E8BAE39673"
    );
}