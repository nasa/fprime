// ======================================================================
// \title Os/test/ut/rawtime/RulesHeaders.hpp
// \brief rule definitions for common testing
// ======================================================================

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__
#include <gtest/gtest.h>
#include <vector>

#include "Os/RawTime.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/BoundedScenario.hpp"


// #include "STest/Scenario/Scenario.hpp"

#include <chrono>

namespace Os {
namespace Test {
namespace RawTime {

struct Tester {
    // Constructors that ensures the mutex is always valid
    Tester(): m_times() {}

    // Destructor must be virtual
    virtual ~Tester() = default;

    static constexpr U32 TIME_COUNT = 5;

    //! RawTime under test
    std::vector<Os::RawTime> m_times;

    //! Shadow time for testing (vector of std time points)
    std::vector<std::chrono::time_point<std::chrono::system_clock>> m_shadow_times;

    //! Get time for shadow state, at specified index
    void shadow_getRawTime(FwIndexType index) {
        this->m_shadow_times[index] = std::chrono::system_clock::now();
    }

    U32 shadow_getDiffUsec(std::chrono::time_point<std::chrono::system_clock>& t1, std::chrono::time_point<std::chrono::system_clock>& t2) const {
        return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t2).count();
    }

    FwIndexType pick_random_index() const {
        return STest::Pick::lowerUpper(0, TIME_COUNT - 1);;
    }

// Do NOT alter, adds rules to Tester as inner classes
#include "RawTimeRules.hpp"
};

}  // namespace RawTime
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
