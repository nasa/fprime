// ======================================================================
// \title  ComBufferToComAdapterTestMain.cpp
// \brief  cpp file for ComBufferToComAdapter component test main function
// ======================================================================

#include "ComBufferToComAdapterTester.hpp"

TEST(Nominal, PrependsDescriptor) {
    Svc::ComBufferToComAdapterTester tester;
    tester.testNominal();
}

TEST(Nominal, EmptyPayload) {
    Svc::ComBufferToComAdapterTester tester;
    tester.testEmptyPayload();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
