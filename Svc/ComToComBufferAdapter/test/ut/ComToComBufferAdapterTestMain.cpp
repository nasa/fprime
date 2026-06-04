// ======================================================================
// \title  ComToComBufferAdapterTestMain.cpp
// \brief  cpp file for ComToComBufferAdapter component test main function
// ======================================================================

#include "ComToComBufferAdapterTester.hpp"

TEST(Nominal, StripsDescriptor) {
    Svc::ComToComBufferAdapterTester tester;
    tester.testNominal();
}

TEST(Nominal, EmptyPayload) {
    Svc::ComToComBufferAdapterTester tester;
    tester.testEmptyPayload();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
