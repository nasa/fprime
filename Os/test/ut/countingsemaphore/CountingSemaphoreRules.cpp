// ======================================================================
// \title Os/test/ut/countingsemaphore/CountingSemaphoreRules.cpp
// \brief rule implementations for CountingSemaphore
// ======================================================================
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
    FW_ASSERT(status == Os::CountingSemaphore::Status::OP_OK, status);
}

Tester::WaitTimeout::WaitTimeout(AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester>& runner)
    : ConcurrentRule<Os::Test::CountingSemaphore::Tester>("WaitTimeout", runner) {}

bool Tester::WaitTimeout::precondition(const Tester& state) {
    return true;
}

void Tester::WaitTimeout::action(Tester& state) {
    ++state.waiters;
    Os::CountingSemaphore::Status status = state.semaphore.waitTimeout(100);
    --state.waiters;
    FW_ASSERT(status == Os::CountingSemaphore::Status::OP_OK || status == Os::CountingSemaphore::Status::ERROR_TIMEOUT,
              status);
}

Tester::Post::Post(AggregatedConcurrentRule<Os::Test::CountingSemaphore::Tester>& runner)
    : ConcurrentRule<Os::Test::CountingSemaphore::Tester>("Post", runner) {}

bool Tester::Post::precondition(const Tester& state) {
    return true;
}

void Tester::Post::action(Tester& state) {
    this->wait_for_next_step();
    Os::CountingSemaphore::Status status = state.semaphore.post();
    FW_ASSERT(status == Os::CountingSemaphore::Status::OP_OK, status);
}

}  // namespace CountingSemaphore
}  // namespace Test
}  // namespace Os
