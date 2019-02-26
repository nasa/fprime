#include "Tester.hpp"

TEST(Nominal,NominalInit) {

    Svc::Tester tester;
    tester.testAsserts();

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
