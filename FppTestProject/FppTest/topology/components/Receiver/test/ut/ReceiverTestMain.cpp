// ======================================================================
// \title  ReceiverTestMain.cpp
// \author bocchino
// \brief  cpp file for Receiver component test main function
// ======================================================================

#include "ReceiverTester.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, receiveData) {
    FppTest::ReceiverTester tester;
    tester.receiveData();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
