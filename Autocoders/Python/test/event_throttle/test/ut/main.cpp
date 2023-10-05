#include "event_throttleTester.hpp"

TEST(EventThrottleTest,ThrottleTest) {
    Somewhere::event_throttleTester tester;
    tester.doEventThrottleTest();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
