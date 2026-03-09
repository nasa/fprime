// ======================================================================
// \title Os/test/ut/rwmutex/RulesHeaders.hpp
// \brief rule definitions for common testing
// ======================================================================

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__
#include <gtest/gtest.h>
#include "Os/RwMutex.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/Scenario.hpp"

namespace Os {
namespace Test {
namespace RwMutex {

struct Tester {
    //! State representation of a RwMutex.
    //!
    enum RwMutexState {
        UNINITIALIZED,  //!< Mutex is uninitialized
        UNLOCKED,       //!< Mutex is unlocked
        READ_LOCKED,    //!< One or more readers hold shared access
        WRITE_LOCKED    //!< Single writer holds exclusive access
    };

    //! Assert in RwMutex.cpp for searching death text
    static constexpr const char* ASSERT_IN_RWMUTEX_CPP = "Assert: \".*/Os/.*/RwMutex\\.cpp:[0-9]+\"";

    // Constructors that ensures the rwmutex is always valid
    Tester() : m_rwmutex(), m_state(UNLOCKED) {}

    // Destructor must be virtual
    virtual ~Tester() = default;

    //! RwMutex under test
    Os::RwMutex m_rwmutex;

    //! Shared value protected by the RwMutex for testing purposes
    int m_value = 0;

    //! RwMutex state, for testing purposes
    RwMutexState m_state = UNINITIALIZED;

    //! Number of active shared (read) lock holders
    FwSizeType m_reader_count = 0;

// Do NOT alter, adds rules to Tester as inner classes
#include "RwMutexRules.hpp"
};

}  // namespace RwMutex
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
