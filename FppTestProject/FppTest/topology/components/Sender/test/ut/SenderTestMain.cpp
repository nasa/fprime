// ======================================================================
// \title  SenderTestMain.cpp
// \author bocchino
// \brief  cpp file for Sender component test main function
// ======================================================================

#include "SenderTester.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, sendData) {
    FppTest::SenderTester tester;
    tester.sendData();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
