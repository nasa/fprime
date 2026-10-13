// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "RateLimiterTester.hpp"
#include "STest/Random/Random.hpp"
#include "TokenBucketTester.hpp"

TEST(RateLimiterTest, TestCounterTriggering) {
    Utils::RateLimiterTester tester;
    tester.testCounterTriggering();
}

TEST(RateLimiterTest, TestTimeTriggering) {
    Utils::RateLimiterTester tester;
    tester.testTimeTriggering();
}

TEST(RateLimiterTest, TestCounterAndTimeTriggering) {
    Utils::RateLimiterTester tester;
    tester.testCounterAndTimeTriggering();
}

TEST(RateLimiterTest, TestTimeBase) {
    Utils::RateLimiterTester tester;
    tester.testTimeBase();
}

TEST(RateLimiterTest, TestDefaultConstructorAndSetters) {
    Utils::RateLimiterTester tester;
    tester.testDefaultConstructorAndSetters();
}

TEST(TokenBucketTest, TestTriggering) {
    Utils::TokenBucketTester tester;
    tester.testTriggering();
}

TEST(TokenBucketTest, TestReconfiguring) {
    Utils::TokenBucketTester tester;
    tester.testReconfiguring();
}

TEST(TokenBucketTest, TestInitialSettings) {
    Utils::TokenBucketTester tester;
    tester.testInitialSettings();
}

TEST(TokenBucketTest, TestTimeBase) {
    Utils::TokenBucketTester tester;
    tester.testTimeBase();
}

TEST(TokenBucketTest, TestReplenishAndEdgeCases) {
    Utils::TokenBucketTester tester;
    tester.testReplenishAndEdgeCases();
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
