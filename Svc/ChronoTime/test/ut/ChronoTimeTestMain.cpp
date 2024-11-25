// ======================================================================
// \title  ChronoTimeTestMain.cpp
// \author mstarch
// \brief  cpp file for ChronoTime component test main function
// ======================================================================

#include "ChronoTimeTester.hpp"

TEST(Nominal, TestBasicTime) {
    Svc::ChronoTimeTester tester;
    tester.test_basic_time();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
