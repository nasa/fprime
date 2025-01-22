// ======================================================================
// \title Os/test/ut/rawtime/RulesHeaders.hpp
// \brief rule definitions for common testing
// ======================================================================

#ifndef __RULES_HEADERS__
#define __RULES_HEADERS__
#include <gtest/gtest.h>
#include <vector>
#include <chrono>

#include "Os/RawTime.hpp"
#include "STest/Rule/Rule.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/RandomScenario.hpp"
#include "STest/Scenario/BoundedScenario.hpp"



namespace Os {
namespace Test {
namespace RawTime {

struct Tester {
    // Constructors that ensures the mutex is always valid
    Tester() = default;

    // Destructor must be virtual
    virtual ~Tester() = default;

    // Number of instances of RawTime under test
    static constexpr U32 TEST_TIME_COUNT = 5;

    // Threshold for time differences, in microseconds
    // This value was selected empirically, some platforms may need to adjust
    static constexpr U32 INTERVAL_DIFF_THRESHOLD = 20;

    //! RawTime (array thereof) under test
    std::vector<Os::RawTime> m_times;

    //! Shadow time for testing (vector of std time points)
    std::vector<std::chrono::time_point<std::chrono::system_clock>> m_shadow_times;

    //! Get time for shadow state, at specified index
    void shadow_now(FwIndexType index) {
        this->m_shadow_times[index] = std::chrono::system_clock::now();
    }

    U32 shadow_getDiffUsec(std::chrono::time_point<std::chrono::system_clock>& t1, std::chrono::time_point<std::chrono::system_clock>& t2) const {
        // Signedness is important here so we compare and substract accordingly
        if (t1 < t2) {
            return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
        }
        return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t2).count();
    }

    void shadow_getTimeInterval(FwIndexType index1, FwIndexType index2, Fw::TimeInterval& interval) {
        auto duration = this->m_shadow_times[index1] - this->m_shadow_times[index2];
        if (duration < std::chrono::system_clock::duration::zero()) {
            duration = -duration;
        }

        U32 microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
        U32 seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();

        interval.set(seconds, microseconds);
    }

    void shadow_validate_interval_result(FwIndexType index1, FwIndexType index2, const Fw::TimeInterval& interval) {
        Fw::TimeInterval shadow_interval;
        Fw::TimeInterval result;
        this->shadow_getTimeInterval(index1, index2, shadow_interval);
        // Signedness is important here so we compare and substract accordingly
        if (interval < shadow_interval) {
            result = Fw::TimeInterval::sub(shadow_interval, interval);
        } else {
            result = Fw::TimeInterval::sub(interval, shadow_interval);
        }
        // Check that difference between 2 intervals is less than threshold
        ASSERT_TRUE(result < Fw::TimeInterval(0, INTERVAL_DIFF_THRESHOLD)) 
            << "Interval difference: " << result;
    }

    void shadow_validate_diff_result(U32 result, U32 shadow_result) {
        U32 result_diff;
        if (result > shadow_result) {
            result_diff = result - shadow_result;
        } else {
            result_diff = shadow_result - result;
        }
        ASSERT_TRUE(result_diff < INTERVAL_DIFF_THRESHOLD)
            << "Difference between results: " << result_diff << " microseconds";
    }

    FwIndexType pick_random_index() const {
        return STest::Pick::lowerUpper(0, TEST_TIME_COUNT - 1);;
    }

// Do NOT alter, adds rules to Tester as inner classes
#include "RawTimeRules.hpp"
};

}  // namespace RawTime
}  // namespace Test
}  // namespace Os
#endif  // __RULES_HEADERS__
