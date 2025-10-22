// ======================================================================
// \title Os/test/ut/rawtime/RulesHeaders.hpp
// \brief rule definitions for common testing
// ======================================================================

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__
#include <gtest/gtest.h>
#include <chrono>
#include <vector>

#include "Os/RawTime.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Scenario/BoundedScenario.hpp"
#include "STest/Scenario/RandomScenario.hpp"

namespace Os {
namespace Test {
namespace RawTime {

//! ------------------------------------------------------------------------------------------------------
//! REQUIRED NOTICE TO PLATFORM IMPLEMENTORS
//! ------------------------------------------------------------------------------------------------------
//! The rules defined in Os/test/ut/rawtime are intended to be reusable, with one condition:
//! implementors of an OSAL must provide their own implementation of the `assert_and_update_now()` helper.
//!
//! This is because the "Now" rule must keep a Os::RawTime object synchronized with a "shadow" (acting as a
//! tracker) time object (std::time_point), which is not possible without having knowledge of the platform
//! specific time representation.
//!
//! The assert_and_update_now() method *must* therefore be implemented and have the following behavior:
//! 1. Verify that raw_time_under_test represents a time between lower_time and upper_time
//! 2. Update shadow_time reference to represent the same time as raw_time_under_test
//!
//! An example of such implementation can be found in Os/Posix/test/ut/PosixRawTimeTests.cpp.
//!
//! If this is not possible for your OSAL implementation, it is recommended to write tests directly
//! for your platform.
//!
//! \param raw_time_under_test The RawTime object under test
//! \param lower_time The lower bound time point
//! \param upper_time The upper bound time point
//! \param shadow_time reference to shadow time to update
void assert_and_update_now(const Os::RawTime& raw_time_under_test,
                           const std::chrono::system_clock::time_point& lower_time,
                           const std::chrono::system_clock::time_point& upper_time,
                           std::chrono::system_clock::time_point& shadow_time  //!< reference to shadow time to update
);

struct Tester {
    // Constructors that ensures the mutex is always valid
    Tester() = default;

    // Destructor must be virtual
    virtual ~Tester() = default;

    // Number of instances of RawTime under test
    static constexpr U32 TEST_TIME_COUNT = 5;

    //! RawTime (array thereof) under test
    std::vector<Os::RawTime> m_times;

    //! Shadow time for testing (vector of std time points)
    std::vector<std::chrono::time_point<std::chrono::system_clock>> m_shadow_times;

    //! Get time for shadow state, at specified index
    void shadow_now(FwIndexType index) { this->m_shadow_times[index] = std::chrono::system_clock::now(); }

    U32 shadow_getDiffUsec(std::chrono::time_point<std::chrono::system_clock>& t1,
                           std::chrono::time_point<std::chrono::system_clock>& t2) const {
        // Signedness is important here so we compare and substract accordingly
        if (t1 < t2) {
            return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
        return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t2).count();
    }

    //! Compute the time interval between two shadow times at specified indices and populate the given interval
    //! with the result
    void shadow_getTimeInterval(FwIndexType index1, FwIndexType index2, Fw::TimeInterval& interval) {
        auto duration = this->m_shadow_times[index1] - this->m_shadow_times[index2];
        if (duration < std::chrono::system_clock::duration::zero()) {
            duration = -duration;
        }
        U32 microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000;
        U32 seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        interval.set(seconds, microseconds);
    }

    FwIndexType pick_random_index() const { return STest::Pick::lowerUpper(0, TEST_TIME_COUNT - 1); }

// Do NOT alter, adds rules to Tester as inner classes
#include "RawTimeRules.hpp"
};

}  // namespace RawTime
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
