// ======================================================================
// \title  FrameAccumulatorTestMain.cpp
// \author thomas-bc
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

TEST(FrameAccumulator, TestReceiveZeroSizeBuffer) {
    Svc::FrameAccumulatorTester tester;
    tester.testReceiveZeroSizeBuffer();
}

TEST(FrameAccumulator, TestAccumulateTwoBuffers) {
    Svc::FrameAccumulatorTester tester;
    tester.testAccumulateTwoBuffers();
}

TEST(FrameAccumulator, testAccumulateBuffersEmitFrame) {
    Svc::FrameAccumulatorTester tester;
    tester.testAccumulateBuffersEmitFrame();
}

TEST(FrameAccumulator, testAccumulateBuffersEmitManyFrames) {
    Svc::FrameAccumulatorTester tester;
    tester.testAccumulateBuffersEmitManyFrames();
}

TEST(FrameAccumulator, testBufferReturnDeallocation) {
    Svc::FrameAccumulatorTester tester;
    tester.testBufferReturnDeallocation();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
