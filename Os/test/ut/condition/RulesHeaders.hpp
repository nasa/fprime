// ======================================================================
// \title Os/test/ut/condition/RulesHeaders.cpp
// \brief condition test rules headers
// ======================================================================

#ifndef OS_TEST_CONDITION_RULES_HEADERS__
#define OS_TEST_CONDITION_RULES_HEADERS__

#include "Os/Condition.hpp"
#include "Os/test/ConcurrentRule.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"


namespace Os {
namespace Test {
namespace Condition {

struct Tester {
  public:
    //! Constructor
    Tester() = default;
    virtual ~Tester() = default;

    Os::ConditionVariable m_condition;
    bool m_notify = false;
    FwSizeType m_waiters = 0;

  public:
#include "ConditionRules.hpp"
};

}  // namespace Queue
}  // namespace Test
}  // namespace Os

#endif
