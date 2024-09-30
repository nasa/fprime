// ======================================================================
// \title Os/test/ut/condition/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/condition/CommonTests.hpp"
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/test/ConcurrentRule.hpp"
#include "Os/test/ut/condition/RulesHeaders.hpp"


TEST(Blocking, WaitSingle) {
    Os::Test::Condition::Tester tester;
    AggregatedConcurrentRule<Os::Test::Condition::Tester> aggregator;
    Os::Test::Condition::Tester::Wait wait_rule(aggregator);
    Os::Test::Condition::Tester::Notify notify_rule(aggregator);

    aggregator.apply(tester);
    aggregator.join();
}

TEST(Blocking, WaitAll) {
    Os::Test::Condition::Tester tester;
    AggregatedConcurrentRule<Os::Test::Condition::Tester> aggregator;
    PseudoRule<Os::Test::Condition::Tester> pseudoRule("Notify", aggregator);
    Os::Test::Condition::Tester::Wait wait_rule1(aggregator);
    Os::Test::Condition::Tester::Wait wait_rule2(aggregator);
    Os::Test::Condition::Tester::NotifyAll notify_rule(aggregator);
    aggregator.apply(tester);
    {
        Os::ScopeLock lock(aggregator.getLock());
        while (tester.m_waiters != 2) {
            pseudoRule.wait_for_next_step();
        }
    }
    std::string to_notify("NotifyAll");
    aggregator.notify(to_notify);
    {
        Os::ScopeLock lock(aggregator.getLock());
        while (tester.m_waiters > 0) {
            pseudoRule.wait_for_next_step();
        }
    }
    aggregator.notify(to_notify);
    aggregator.join();
}
