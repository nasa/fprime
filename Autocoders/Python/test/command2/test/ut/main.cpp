// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "command2Tester.hpp"

TEST(Test, Miscellaneous) {
    AcTest::command2Tester tester;
    tester.msgTest();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
