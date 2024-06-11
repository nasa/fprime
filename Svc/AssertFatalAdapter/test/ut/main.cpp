#include "AssertFatalAdapterTester.hpp"

TEST(Nominal,NominalInit) {

    Svc::AssertFatalAdapterTester tester;
    tester.testAsserts();

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
