// ======================================================================
// \title Os/test/ut/condition/ConditionRules.cpp
// \brief condition variables rule implementations
// ======================================================================

#include "CommonTests.hpp"
#include "Fw/Types/String.hpp"


// ------------------------------------------------------------------------------------------------------
// Rule:  Wait
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Condition::Tester::Wait::Wait(AggregatedConcurrentRule<Os::Test::Condition::Tester>& runner)
    : ConcurrentRule<Os::Test::Condition::Tester>("Wait", runner) {}

bool Os::Test::Condition::Tester::Wait::precondition(const Os::Test::Condition::Tester& state  //!< The test state
) {
    return true;
}

void Os::Test::Condition::Tester::Wait::action(Os::Test::Condition::Tester& state  //!< The test state
) {
    state.m_notify = false;
    state.m_waiters += 1;
    this->notify_other("Notify");
    state.m_condition.wait(this->getLock());
    ASSERT_TRUE(state.m_notify) << "Notify was not set";
    state.m_waiters -= 1;
    this->notify_other("Notify");
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Notify
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Condition::Tester::Notify::Notify(AggregatedConcurrentRule<Os::Test::Condition::Tester>& runner)
    : ConcurrentRule<Os::Test::Condition::Tester>("Notify", runner) {}

bool Os::Test::Condition::Tester::Notify::precondition(const Os::Test::Condition::Tester& state  //!< The test state
) {
    return true;
}

void Os::Test::Condition::Tester::Notify::action(Os::Test::Condition::Tester& state  //!< The test state
) {
    FwSizeType waiters = 0;
    this->wait_for_next_step();
    waiters = state.m_waiters;
    state.m_notify = true;
    state.m_condition.notify();
    // Wait for notification and ensure exactly one waiter woke up
    this->wait_for_next_step();
    ASSERT_EQ(waiters - 1, state.m_waiters);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  NotifyAll
//
// ------------------------------------------------------------------------------------------------------

Os::Test::Condition::Tester::NotifyAll::NotifyAll(AggregatedConcurrentRule<Os::Test::Condition::Tester>& runner)
    : ConcurrentRule<Os::Test::Condition::Tester>("NotifyAll", runner) {}

bool Os::Test::Condition::Tester::NotifyAll::precondition(const Os::Test::Condition::Tester& state  //!< The test state
) {
    return true;
}

void Os::Test::Condition::Tester::NotifyAll::action(Os::Test::Condition::Tester& state  //!< The test state
) {
    this->wait_for_next_step();
    state.m_notify = true;
    state.m_condition.notifyAll();
    this->wait_for_next_step();
    ASSERT_EQ(0, state.m_waiters);
}
