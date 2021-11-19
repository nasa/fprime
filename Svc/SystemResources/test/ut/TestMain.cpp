// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, Telemetry) {
    Svc::Tester tester;
    tester.test_tlm();
}

TEST(OffNominal, Disabled) {
    Svc::Tester tester;
    tester.test_disable_enable();
}

TEST(Nominal, Events) {
    Svc::Tester tester;
    tester.test_version_evr();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
