#include <client/client.h>
#include <gtest/gtest.h>
#include <client/network_Can_generated.h>

TEST(ClientTests, TestConnection)
{
    bool actual = formConnection();
    ASSERT_FALSE(actual);
}
TEST(ClientTests, TestReceiveData)
{
    auto metaData = GetRegisterFile(answer);
    auto bufferData = metaData->buffer();
    auto frameData = bufferData->Get(0)->frame();
    auto client_payload = frameData->payload();
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
    
}