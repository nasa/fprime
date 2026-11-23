// ----------------------------------------------------------------------
// Main.cpp
// ----------------------------------------------------------------------

#include <Fw/Test/UnitTest.hpp>
#include <chrono>
#include <csignal>
#include <pthread.h>
#include <thread>
#include "LinuxTimerTester.hpp"

TEST(Nominal, InitTest) {
    TEST_CASE(103.1.1, "Cycle Test");
    Svc::LinuxTimerTester tester;
    tester.runCycles();
}

TEST(Nominal, InterruptedReadTest) {
    Svc::LinuxTimerTester tester;
    // Install a handler without SA_RESTART so the blocking timerfd read
    // fails with EINTR instead of resuming when the signal arrives
    struct sigaction action {};
    action.sa_handler = [](int) {};
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    ASSERT_EQ(::sigaction(SIGUSR1, &action, nullptr), 0);
    std::thread timerThread([&tester]() { tester.runInterruptedRead(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(::pthread_kill(timerThread.native_handle(), SIGUSR1), 0);
    // The first genuine tick is not due until ~1s; no tick may arrive early
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    EXPECT_EQ(tester.getTickCount(), 0u);
    // The first genuine tick quits the timer; the thread exits once the
    // following read observes the quit flag
    timerThread.join();
    EXPECT_EQ(tester.getTickCount(), 1u);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
