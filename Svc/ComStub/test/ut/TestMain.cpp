// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, Initial) {
    Svc::Tester tester;
    tester.test_initial();
}

TEST(Nominal, BasicIo) {
    Svc::Tester tester;
    tester.test_basic();
}


TEST(Nominal, Fail) {
    Svc::Tester tester;
    tester.test_fail();
}

TEST(OffNominal, Retry) {
    Svc::Tester tester;
    tester.test_retry();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
