
#include <gtest/gtest.h>
#include "HexBase.hpp"

#include <string>

TEST(HexBase, Hex_to_Bytes_to_Hex){
    std::string hex = "CF83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E";
    PQB::byte *bytes = new PQB::byte[64];
    PQB::hexStringToBytes(hex, bytes, 64);
    ASSERT_TRUE(PQB::bytesToHexString(bytes, 64) == hex);
}

TEST(HexBase, Hex_to_Bytes_to_Hex_Invalid){
    std::string hex = "CX83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA3E";
    PQB::byte *bytes = new PQB::byte[64];
    PQB::hexStringToBytes(hex, bytes, 64);
    std::string finalHex = PQB::bytesToHexString(bytes, 64);
    ASSERT_FALSE(finalHex == hex);
    ASSERT_TRUE(finalHex == "C83E1357EEFB8BDF1542850D66D8007D620E4050B5715DC83F4A921D36CE9CE47D0D13C5D85F2B0FF8318D2877EEC2F63B931BD47417A81A538327AF927DA300");
}

