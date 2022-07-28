// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, Com) {
    Svc::Tester tester;
    tester.test_com();
}

TEST(Nominal, Buffer) {
    Svc::Tester tester;
    tester.test_buffer();
}

TEST(Nominal, ManySends) {
    Svc::Tester tester;
    tester.test_com(27);
    tester.test_buffer(27);
    tester.test_com(31);
    tester.test_buffer(31);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
