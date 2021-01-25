// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, Setup) {
    Svc::Tester tester;
    tester.testSetup();
}

TEST(Nominal, OneSize) {
    Svc::Tester tester;
    tester.oneBufferSize();
}

TEST(Nominal, MultSize) {
    Svc::Tester tester;
    tester.multBuffSize();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
