// ======================================================================
// \title Os/Posix/test/ut/PosixRawTimeTests.cpp
// \brief tests for posix implementation for Os::RawTime
// ======================================================================
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Posix/RawTime.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/test/ut/rawtime/CommonTests.hpp"
#include "Os/test/ut/rawtime/RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/Scenario.hpp"

// namespace RawTimeTestHelper {

bool operator==(const Os::RawTime& lhs, const std::__1::chrono::system_clock::time_point& rhs) {
    const Os::Posix::RawTime::PosixRawTimeHandle* ts =
        static_cast<const Os::Posix::RawTime::PosixRawTimeHandle*>(const_cast<Os::RawTime&>(lhs).getHandle());
    // Implementation needed
    return (ts->m_timespec.tv_sec == std::chrono::duration_cast<std::chrono::seconds>(rhs.time_since_epoch()).count() &&
            ts->m_timespec.tv_nsec ==
                std::chrono::duration_cast<std::chrono::nanoseconds>(rhs.time_since_epoch()).count() % 1000000000);
}

bool operator>=(const Os::RawTime& lhs, const std::__1::chrono::system_clock::time_point& rhs);
bool operator<=(const Os::RawTime& lhs, const std::__1::chrono::system_clock::time_point& rhs);

// }  // namespace RawTimeTestHelper

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
