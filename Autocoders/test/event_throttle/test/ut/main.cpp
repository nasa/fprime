#include <Tester.hpp>

TEST(EventThrottleTest,ThrottleTest) {
    Somewhere::Tester tester;
    tester.doEventThrottleTest();
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
