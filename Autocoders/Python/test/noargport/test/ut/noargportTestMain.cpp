// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "noargportTester.hpp"

TEST(Nominal, TestNoArgs) {
    ExampleComponents::noargportTester tester;
    tester.testNoArgs();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
