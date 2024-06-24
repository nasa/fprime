// ======================================================================
// \title Os/test/ut/Mutex/CommonTests.hpp
// \brief definitions used in common Mutex testing
// ======================================================================
#include <Os/Mutex.hpp>
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

    //! Setup function delegating to UT setUp function
    void SetUp() override;

    //! Setup function delegating to UT tearDown function
    void TearDown() override;

    //! Tester/state implementation
    std::unique_ptr<Os::Test::Mutex::Tester> tester;
};

#endif  // OS_TEST_UT_COMMON_MUTEX_TESTS_HPP
