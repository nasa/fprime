#include "Tester.hpp"
#include "STest/Random/Random.hpp"

TEST(Nominal, AddCommand) {
	Ref::Tester tester;
	tester.testAddCommand();
}

TEST(Nominal, SubCommand) {
	Ref::Tester tester;
	tester.testSubCommand();
}

TEST(Nominal, MulCommand) {
	Ref::Tester tester;
	tester.testMulCommand();
}

TEST(Nominal, DivCommand) {
	Ref::Tester tester;
	tester.testDivCommand();
}

TEST(Nominal, Result) {
    Ref::Tester tester;
    tester.testResult();
}

int main(int argc, char ** argv) {
	
	::testing::InitGoogleTest(&argc, argv);

	//set seed for picking numbers
	STest::Random::seed();

	return RUN_ALL_TESTS();
}
