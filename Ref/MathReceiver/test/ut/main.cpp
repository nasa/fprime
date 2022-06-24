#include "Tester.hpp" 
#include "STest/Random/Random.hpp"

TEST(Nominal, AddOp) {
    Ref::Tester tester;
    tester.testAdd();
}

TEST(Nominal, SubOp) {
    Ref::Tester tester;
    tester.testSub();
}

TEST(Nominal, MulOp) {
    Ref::Tester tester;
    tester.testMul();
}

TEST(Nominal, DivOp) {
    Ref::Tester tester;
    tester.testDiv();
}

TEST(Nominal, Throttle) {
    Ref::Tester tester;
    tester.testThrottle();
}

int main(int argc, char **argv) { 

	  ::testing::InitGoogleTest(&argc, argv); 

	  STest::Random::seed();

	    return RUN_ALL_TESTS(); 

} 
