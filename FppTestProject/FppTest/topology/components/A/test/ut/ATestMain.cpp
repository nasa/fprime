// ======================================================================
// \title  ATestMain.cpp
// \author bocchino
// \brief  cpp file for A component test main function
// ======================================================================

#include "ATester.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, sendData) {
    FppTest::ATester tester;
    tester.sendData();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
