// ======================================================================
// \title Os/test/ut/countingsemaphore/CountingSemaphoreRules.cpp
// \brief rule implementations for CountingSemaphore
// ======================================================================
#include <gtest/gtest.h>
#include "Fw/Time/TimeInterval.hpp"
#include "Os/test/ConcurrentRule.hpp"
#include "Os/test/ut/countingsemaphore/RulesHeaders.hpp"

namespace Os {
namespace Test {
namespace CountingSemaphore {

Tester::Wait::Wait(AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester>& runner)
    : ConcurrentRule<Os::Test::CountingSemaphore::Tester>("Wait", runner) {}

bool Tester::Wait::precondition(const Tester& state) {
    return true;
}

void Tester::Wait::action(Tester& state) {
    ++state.waiters;
    Os::CountingSemaphore::Status status = state.semaphore.wait();
    --state.waiters;
    ASSERT_EQ(status, Os::CountingSemaphore::Status::OP_OK);
}
}

Tester::WaitTimeout::WaitTimeout(AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester>& runner)
    : ConcurrentRule<Os::Test::CountingSemaphore::Tester>("WaitTimeout", runner) {}

bool Tester::WaitTimeout::precondition(const Tester& state) {
    return true;
}

void Tester::WaitTimeout::action(Tester& state) {
    ++state.waiters;
    Fw::TimeInterval timeout(0, 100000);  // 100ms
    Os::CountingSemaphore::Status status = state.semaphore.waitTimeout(timeout);
    --state.waiters;
    ASSERT_TRUE(status == Os::CountingSemaphore::Status::OP_OK || status == Os::CountingSemaphore::Status::ERROR_TIMEOUT)
        << "Unexpected status: " << status;
}

Tester::Post::Post(AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester>& runner)
    : ConcurrentRule<Os::Test::CountingSemaphore::Tester>("Post", runner) {}

bool Tester::Post::precondition(const Tester& state) {
    return true;
}

void Tester::Post::action(Tester& state) {
    this->wait_for_next_step();
    Os::CountingSemaphore::Status status = state.semaphore.post();
    ASSERT_EQ(status, Os::CountingSemaphore::Status::OP_OK);
}

}  // namespace CountingSemaphore
}  // namespace Test
}  // namespace Os
