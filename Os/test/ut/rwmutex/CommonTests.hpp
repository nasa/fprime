// ======================================================================
// \title Os/test/ut/rwmutex/CommonTests.hpp
// \brief GoogleTest fixture definitions used in common RwMutex testing
// ======================================================================
#include <gtest/gtest.h>
#include <Os/RwMutex.hpp>
#include <Os/test/ut/rwmutex/RulesHeaders.hpp>

#ifndef OS_TEST_UT_COMMON_RWMUTEX_TESTS_HPP
#define OS_TEST_UT_COMMON_RWMUTEX_TESTS_HPP
namespace Os {
namespace Test {
namespace RwMutex {}  // namespace RwMutex
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
    std::unique_ptr<Os::Test::RwMutex::Tester> tester;
};

#endif  // OS_TEST_UT_COMMON_RWMUTEX_TESTS_HPP
