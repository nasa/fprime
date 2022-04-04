#include "Tester.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, AddCommand) {
    Ref::Tester tester;
    tester.testAddCommand();
}
TEST(Nominal, Result) {
    Ref::Tester tester;
    tester.testResult();
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  STest::Random::seed();
  return RUN_ALL_TESTS();
}