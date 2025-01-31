// ======================================================================
// \title  FrameAccumulatorTestMain.cpp
// \author chammard
// \brief  cpp file for FrameAccumulator component test main function
// ======================================================================

#include "FrameAccumulatorTester.hpp"
#include "STest/Random/Random.hpp"

TEST(FrameAccumulator, TestFrameDetected) {
    Svc::FrameAccumulatorTester tester;
    tester.testFrameDetected();
}

TEST(FrameAccumulator, TestMoreDataNeeded) {
    Svc::FrameAccumulatorTester tester;
    tester.testMoreDataNeeded();
}

TEST(FrameAccumulator, TestNoFrameDetected) {
    Svc::FrameAccumulatorTester tester;
    tester.testNoFrameDetected();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
