// ======================================================================
// \title Os/test/ut/directory/CommonTests.hpp
// \brief GoogleTest fixture definitions used in common Directory testing
// ======================================================================
#include <Os/Directory.hpp>
#include <gtest/gtest.h>
#include <Os/test/ut/directory/RulesHeaders.hpp>

#ifndef OS_TEST_UT_COMMON_DIRECTORY_TESTS_HPP
#define OS_TEST_UT_COMMON_DIRECTORY_TESTS_HPP
namespace Os {
namespace Test {
namespace Directory {


}  // namespace Directory
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
    std::unique_ptr<Os::Test::Directory::Tester> tester;
};

#endif  // OS_TEST_UT_COMMON_DIRECTORY_TESTS_HPP
