// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "StaticMemoryTester.hpp"

TEST(Nominal, BasicAllocation) {
    Svc::StaticMemoryTester tester;
    tester.test_allocate();
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
