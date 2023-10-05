// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "BufferManagerTester.hpp"

TEST(Nominal, Setup) {
    Svc::BufferManagerTester tester;
    tester.testSetup();
}

TEST(Nominal, OneSize) {
    Svc::BufferManagerTester tester;
    tester.oneBufferSize();
}

TEST(Nominal, MultSize) {
    Svc::BufferManagerTester tester;
    tester.multBuffSize();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
