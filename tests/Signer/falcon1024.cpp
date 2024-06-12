
#include <gtest/gtest.h>
#include "Log.hpp"
#include "Signer.hpp"

TEST(Falcon1024, Sign){
    PQB::Log::init(); // to avoid segfault from uninitialized logger
    auto ss = PQB::Signer::GetInstance("falcon1024");

    std::string message = "Message to sign";
    PQB::byteBuffer pk;
    PQB::byteBuffer sk;
    PQB::byteBuffer signature;
    
    ss->genKeys(sk, pk);
    ss->sign(signature, (PQB::byte*)message.data(), message.size(), sk);

    EXPECT_TRUE(ss->verify(signature, (PQB::byte*)message.data(), message.size(), pk));

    message = "Altered message";
    EXPECT_FALSE(ss->verify(signature, (PQB::byte*)message.data(), message.size(), pk));

}