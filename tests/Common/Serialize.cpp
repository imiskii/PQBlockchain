
#include <gtest/gtest.h>
#include "Serialize.hpp"

#include "PQBtypedefs.hpp"

struct SerializeTest : public testing::Test{
    PQB::byteBuffer buffer;
    size_t offset;

    char c;
    std::uint32_t num1;
    std::uint32_t num2; 

    void SetUp() {
        buffer.resize(sizeof(num1) + sizeof(c) + sizeof(num2));
        offset = 0;

        c = 'A';
        num1 = 42;
        num2 = 11; 
    }

    void TearDown() { 
    }
};


TEST_F(SerializeTest, Serialize){
    PQB::serializeField(buffer, offset, num1);
    PQB::serializeField(buffer, offset, c);
    PQB::serializeField(buffer, offset, num2);
    ASSERT_EQ(buffer.size(), (sizeof(num1) + sizeof(c) + sizeof(num2)));
    ASSERT_EQ(*((std::uint32_t *) buffer.data()), 42);
    ASSERT_EQ(*((char *) (buffer.data() + sizeof(num1))), 'A');
    ASSERT_EQ(*((std::uint32_t *) (buffer.data() + sizeof(num1) + sizeof(c))), 11);
}

TEST_F(SerializeTest, Deserialize){
    PQB::serializeField(buffer, offset, num1);
    PQB::serializeField(buffer, offset, c);
    PQB::serializeField(buffer, offset, num2);
    
    std::uint32_t num1_t;
    char c_t;
    std::uint32_t num2_t;
    offset = 0; // reset offset

    PQB::deserializeField(buffer, offset, num1_t);
    PQB::deserializeField(buffer, offset, c_t);
    PQB::deserializeField(buffer, offset, num2_t);
    
    EXPECT_EQ(num1, num1_t);
    EXPECT_EQ(c, c_t);
    EXPECT_EQ(num2, num2_t);
}