// ======================================================================
// \title Os/test/ut/rawtime/CommonTests.hpp
// \brief GoogleTest fixture definitions used in common RawTime testing
// ======================================================================
#ifndef OS_TEST_UT_COMMON_RAWTIME_TESTS_HPP
#define OS_TEST_UT_COMMON_RAWTIME_TESTS_HPP

#include <gtest/gtest.h>
#include <Os/RawTime.hpp>
#include <Os/test/ut/rawtime/RulesHeaders.hpp>

namespace Os {
namespace Test {
namespace RawTime {}  // namespace RawTime
}  // namespace Test
}  // namespace Os

class Functionality : public ::testing::Test {
  public:
    //! Constructor
    Functionality();

    //! SetUp test fixture
    void SetUp() override;

    //! TearDown test fixture for safe destruction
    void TearDown() override;

    //! Tester/state implementation
    std::unique_ptr<Os::Test::RawTime::Tester> tester;
};

// namespace RawTimeTestHelper {

bool operator==(const Os::RawTime& lhs, const std::__1::chrono::system_clock::time_point& rhs);
bool operator>=(const Os::RawTime& lhs, const std::__1::chrono::system_clock::time_point& rhs);
bool operator<=(const Os::RawTime& lhs, const std::__1::chrono::system_clock::time_point& rhs);

// }  // namespace RawTimeTestHelper

#endif  // OS_TEST_UT_COMMON_RAWTIME_TESTS_HPP
