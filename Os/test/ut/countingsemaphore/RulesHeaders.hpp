// ======================================================================
// \title Os/test/ut/countingsemaphore/RulesHeaders.hpp
// \brief rule definitions for CountingSemaphore testing
// ======================================================================
#ifndef OS_TEST_UT_COUNTINGSEMAPHORE_RULES_HEADERS_HPP
#define OS_TEST_UT_COUNTINGSEMAPHORE_RULES_HEADERS_HPP

#include "Os/CountingSemaphore.hpp"
#include "Os/test/ConcurrentRule.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"

namespace Os {
namespace Test {
namespace CountingSemaphore {

struct Tester {
    Os::CountingSemaphore semaphore;
    U32 waiters;

    Tester() : semaphore(0U), waiters(0U) {}

// Do NOT alter, adds rules to Tester as inner classes
#include "CountingSemaphoreRules.hpp"
};

}  // namespace CountingSemaphore
}  // namespace Test
}  // namespace Os

#endif  // OS_TEST_UT_COUNTINGSEMAPHORE_RULES_HEADERS_HPP
