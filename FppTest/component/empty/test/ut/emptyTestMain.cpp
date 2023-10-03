// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "emptyTester.hpp"

TEST(Nominal, ToDo) {
    emptyTester tester;
    tester.test();
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
