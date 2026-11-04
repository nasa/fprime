// ----------------------------------------------------------------------
// TestMain.cpp
// ----------------------------------------------------------------------

#include "STest/Random/Random.hpp"
#include "StaticMemoryTester.hpp"

TEST(Nominal, BasicAllocation) {
    Svc::StaticMemoryTester tester;
    tester.test_allocate();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
