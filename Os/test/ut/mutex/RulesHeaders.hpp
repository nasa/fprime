// ======================================================================
// \title Os/test/ut/mutex/RulesHeaders.hpp
// \brief rule definitions for common testing
// ======================================================================

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__
#include <gtest/gtest.h>
#include "Os/Mutex.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"

namespace Os {
namespace Test {
namespace Mutex {

struct Tester {
    //! State representation of a Mutex.
    //!
    enum MutexState {
        UNINITIALIZED,  //!< Mutex is uninitialized
        LOCKED,   //!< Mutex is locked
        UNLOCKED  //!< Mutex is unlocked
    };

    //! Assert in Mutex.cpp for searching death text
    static constexpr const char* ASSERT_IN_MUTEX_CPP = "Assert: \".*/Os/.*/Mutex\\.cpp:[0-9]+\"";

    // Constructors that ensures the mutex is always valid
    Tester(): m_mutex(), m_state(UNLOCKED) {}

    // Destructor must be virtual
    virtual ~Tester() = default;

    //! Mutex under test
    Os::Mutex m_mutex;

    //! Shared value protected by the mutex for testing purposes
    int m_value = 0;

    //! Mutex state, for testing purposes
    MutexState m_state = UNINITIALIZED;

// Do NOT alter, adds rules to Tester as inner classes
#include "MutexRules.hpp"
};

}  // namespace Mutex
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
