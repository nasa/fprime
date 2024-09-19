// ======================================================================
// \title Os/test/ut/filesystem/CommonTests.hpp
// \brief GoogleTest fixture definitions used in common FileSystem testing
// ======================================================================
#ifndef OS_TEST_UT_COMMON_FILESYSTEM_TESTS_HPP
#define OS_TEST_UT_COMMON_FILESYSTEM_TESTS_HPP

#include <Os/FileSystem.hpp>
#include <gtest/gtest.h>
#include <Os/test/ut/filesystem/RulesHeaders.hpp>


class Functionality : public ::testing::Test {
  public:
    //! Constructor
    Functionality();

    //! SetUp test fixture
    void SetUp() override;

    //! TearDown test fixture for safe destruction
    void TearDown() override;

    //! Tester/state implementation
    std::unique_ptr<Os::Test::FileSystem::Tester> tester;
};

#endif  // OS_TEST_UT_COMMON_FILESYSTEM_TESTS_HPP
