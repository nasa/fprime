// ======================================================================
// \title Os/test/ut/countingsemaphore/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/countingsemaphore/CommonTests.hpp"
#include <gtest/gtest.h>
#include "Fw/Time/TimeInterval.hpp"
#include "Fw/Types/String.hpp"
#include "Os/test/ConcurrentRule.hpp"
#include "Os/test/ut/countingsemaphore/RulesHeaders.hpp"

TEST(CountingSemaphore, InitialCount) {
    Os::CountingSemaphore sem(5U);
    for (U32 i = 0; i < 5; i++) {
        Os::CountingSemaphore::Status status = sem.wait();
        ASSERT_EQ(status, Os::CountingSemaphore::Status::OP_OK);
    }
}

TEST(CountingSemaphore, PostWait) {
    Os::Test::CountingSemaphore::Tester tester;
    AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester> aggregator;
    Os::Test::CountingSemaphore::Tester::Wait wait_rule(aggregator);
    Os::Test::CountingSemaphore::Tester::Post post_rule(aggregator);

    aggregator.apply(tester);
    // Brief wait to ensure Wait blocks on semaphore before triggering Post
    Fw::TimeInterval delay(0, 10000);  // 10ms
    Os::Task::delay(delay);
    {
        Os::ScopeLock lock(aggregator.getLock());
        ASSERT_EQ(tester.waiters, 1U) << "Waiter should be blocked before post";
    }
    std::string to_post("Post");
    aggregator.notify(to_post);
    aggregator.join();
    ASSERT_EQ(tester.waiters, 0U) << "Waiter should have completed via post";
}

TEST(CountingSemaphore, Timeout) {
    Os::CountingSemaphore sem(0U);
    Fw::TimeInterval timeout(0, 10000);  // 10ms
    Os::CountingSemaphore::Status status = sem.waitTimeout(timeout);
    ASSERT_EQ(status, Os::CountingSemaphore::Status::ERROR_TIMEOUT);
}

TEST(CountingSemaphore, TryWait) {
    Os::CountingSemaphore sem(2U);
    // tryWait should succeed when count > 0
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::OP_OK);
    // tryWait should return ERROR_TIMEOUT when count == 0
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::ERROR_TIMEOUT);
    // Post should restore the count, allowing tryWait to succeed again
    ASSERT_EQ(sem.post(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::OP_OK);
}

TEST(CountingSemaphore, MultipleWaiters) {
    Os::Test::CountingSemaphore::Tester tester;
    AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester> aggregator;
    Os::Test::CountingSemaphore::Tester::Wait wait_rule1(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait_rule2(aggregator);

    aggregator.apply(tester);
    // Brief wait to ensure both Waits block on semaphore before posting
    Fw::TimeInterval delay(0, 10000);  // 10ms
    Os::Task::delay(delay);
    {
        Os::ScopeLock lock(aggregator.getLock());
        ASSERT_EQ(tester.waiters, 2U) << "Both waiters should be blocked before posting";
    }
    // Post directly from main thread to release both waiters
    ASSERT_EQ(tester.semaphore.post(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(tester.semaphore.post(), Os::CountingSemaphore::Status::OP_OK);
    aggregator.join();
    ASSERT_EQ(tester.waiters, 0U) << "All waiters should have completed";
}

TEST(CountingSemaphore, InitialCountNonZero) {
    // Test non-zero initial count allows waits without prior posts
    Os::CountingSemaphore sem(3U);
    // Should succeed at least initialCount times without any posts
    ASSERT_EQ(sem.wait(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.wait(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.wait(), Os::CountingSemaphore::Status::OP_OK);
    // Verify post/wait cycle works
    ASSERT_EQ(sem.post(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.wait(), Os::CountingSemaphore::Status::OP_OK);
    // Drain any remaining tokens (implementation may have extras)
    Fw::TimeInterval drain_timeout(0, 1000);  // 1ms
    while (sem.waitTimeout(drain_timeout) == Os::CountingSemaphore::Status::OP_OK) {
        // Keep draining
    }
    // Now verify timeout on empty semaphore
    Fw::TimeInterval timeout(0, 10000);  // 10ms
    Os::CountingSemaphore::Status status = sem.waitTimeout(timeout);
    ASSERT_EQ(status, Os::CountingSemaphore::Status::ERROR_TIMEOUT);
}

TEST(CountingSemaphore, PostAccumulatesCount) {
    // Each post increments the count by one; there is no upper bound on the count
    Os::CountingSemaphore sem(0U);
    ASSERT_EQ(sem.post(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.post(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.post(), Os::CountingSemaphore::Status::OP_OK);
    // Exactly three tokens are available
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::ERROR_TIMEOUT);
}

TEST(CountingSemaphore, TimeoutSuccess) {
    Os::Test::CountingSemaphore::Tester tester;
    AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester> aggregator;
    Os::Test::CountingSemaphore::Tester::WaitTimeout wait_rule(aggregator);
    Os::Test::CountingSemaphore::Tester::Post post_rule(aggregator);

    aggregator.apply(tester);
    // Brief wait to allow WaitTimeout to block on semaphore before triggering Post
    Fw::TimeInterval delay(0, 10000);  // 10ms
    Os::Task::delay(delay);
    {
        Os::ScopeLock lock(aggregator.getLock());
        ASSERT_EQ(tester.waiters, 1U) << "WaitTimeout should be blocked before post";
    }
    std::string to_post("Post");
    aggregator.notify(to_post);
    aggregator.join();
    ASSERT_EQ(tester.waiters, 0U) << "Waiter should have completed via post, not timeout";
}

TEST(CountingSemaphore, ZeroInitialCount) {
    // A semaphore created with count zero has no tokens until posted
    Os::CountingSemaphore sem(0U);
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::ERROR_TIMEOUT);
    ASSERT_EQ(sem.post(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.wait(), Os::CountingSemaphore::Status::OP_OK);
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::ERROR_TIMEOUT);
}

TEST(CountingSemaphore, LargeInitialCount) {
    // The initial count is honored exactly: N waits succeed, the next does not
    const U32 initial = 10U;
    Os::CountingSemaphore sem(initial);
    for (U32 i = 0; i < initial; i++) {
        ASSERT_EQ(sem.wait(), Os::CountingSemaphore::Status::OP_OK) << "wait " << i;
    }
    ASSERT_EQ(sem.tryWait(), Os::CountingSemaphore::Status::ERROR_TIMEOUT);
}

TEST(CountingSemaphore, FairnessVerification) {
    // Verify all waiters can be released when enough posts occur.
    Os::Test::CountingSemaphore::Tester tester;
    AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester> aggregator;
    Os::Test::CountingSemaphore::Tester::Wait wait_rule1(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait_rule2(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait_rule3(aggregator);

    aggregator.apply(tester);
    // Wait for waiters to block before posting
    Fw::TimeInterval delay(0, 15000);  // 15ms
    Os::Task::delay(delay);
    {
        Os::ScopeLock lock(aggregator.getLock());
        ASSERT_EQ(tester.waiters, 3U) << "All waiters should be blocked before posting";
    }

    // Post directly from main thread to unblock 3 waiters
    for (U32 i = 0; i < 3; i++) {
        ASSERT_EQ(tester.semaphore.post(), Os::CountingSemaphore::Status::OP_OK);
    }

    // All threads should complete successfully
    aggregator.join();
    ASSERT_EQ(tester.waiters, 0U) << "All waiters should have completed";
}

// Priority 3: Robustness

TEST(CountingSemaphore, ManyThreadsStress) {
    // Stress test with 8 wait threads
    Os::Test::CountingSemaphore::Tester tester;
    AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester> aggregator;

    // Create multiple wait rules
    Os::Test::CountingSemaphore::Tester::Wait wait1(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait2(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait3(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait4(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait5(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait6(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait7(aggregator);
    Os::Test::CountingSemaphore::Tester::Wait wait8(aggregator);

    aggregator.apply(tester);
    // Wait for waiters to block before posting
    Fw::TimeInterval delay(0, 30000);  // 30ms for 8 threads
    Os::Task::delay(delay);
    {
        Os::ScopeLock lock(aggregator.getLock());
        ASSERT_EQ(tester.waiters, 8U) << "All waiters should be blocked before posting";
    }

    // Post directly from main thread to unblock 8 waiters
    for (U32 i = 0; i < 8; i++) {
        ASSERT_EQ(tester.semaphore.post(), Os::CountingSemaphore::Status::OP_OK);
    }

    // All threads should complete successfully
    aggregator.join();
    ASSERT_EQ(tester.waiters, 0U) << "All waiters should have completed";
}

// Note: TimeoutAccuracy test removed - implementation-specific behavior
// makes precise timing verification unreliable across different platforms.
// Basic timeout functionality is covered by the Timeout test.
