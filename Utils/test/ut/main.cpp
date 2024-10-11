// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include "RateLimiterTester.hpp"
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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
