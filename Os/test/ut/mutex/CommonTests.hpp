// ======================================================================
// \title Os/test/ut/mutex/CommonTests.hpp
// \brief GoogleTest fixture definitions used in common Mutex testing
// ======================================================================
#include <Os/Mutex.hpp>
#include <gtest/gtest.h>
#include <Os/test/ut/mutex/RulesHeaders.hpp>

#ifndef OS_TEST_UT_COMMON_MUTEX_TESTS_HPP
#define OS_TEST_UT_COMMON_MUTEX_TESTS_HPP
namespace Os {
namespace Test {
namespace Mutex {


}  // namespace Mutex
}  // namespace Test
}  // namespace Os

class FunctionalityTester : public ::testing::Test {
  public:
    //! Constructor
    FunctionalityTester();

    //! SetUp test fixture
    void SetUp() override;

    //! TearDown test fixture for safe destruction
    void TearDown() override;

    //! Tester/state implementation
    std::unique_ptr<Os::Test::Mutex::Tester> tester;
};

#endif  // OS_TEST_UT_COMMON_MUTEX_TESTS_HPP
