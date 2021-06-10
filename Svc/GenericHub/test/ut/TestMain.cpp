// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "Tester.hpp"

TEST(Nominal, TestIo) {
    Svc::Tester tester;
    tester.test_in_out();
}


TEST(Nominal, TestBufferIo) {
    Svc::Tester tester;
    tester.test_buffer_io();
}

TEST(Nominal, TestRandomIo) {
    Svc::Tester tester;
    tester.test_random_io();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
