// ======================================================================
// \title Os/test/ut/directory/CommonTests.hpp
// \brief GoogleTest fixture definitions used in common Directory testing
// ======================================================================
#ifndef OS_TEST_UT_COMMON_DIRECTORY_TESTS_HPP
#define OS_TEST_UT_COMMON_DIRECTORY_TESTS_HPP

#include <Os/Directory.hpp>
#include <gtest/gtest.h>
#include <Os/test/ut/directory/RulesHeaders.hpp>

// ----------------------------------------------------------------------
//! Notes on CommonTests.cpp reusability
//
//! In order to reuse these tests, the implementor of an OSAL implementation shall
//! provide a setUp and tearDown implementation in the Os::Test::Directory namespace
//! An example is in Os/Posix/test/ut/PosixDirectoryTests.cpp
//! The setUp function shall create a test directory and populate it with files
//! The directory and files shall be tracked in the tester object with:
//!   - The path of the test directory shall be assigned to the tester->m_path member
//!   - The name of each file shall be pushed in tester->m_filenames vector
//! There are no requirement on the number of the files, a random number >=0 is recommended
//! The tearDown function shall remove the test directory and all files in it
// ----------------------------------------------------------------------

namespace Os {
namespace Test {
namespace Directory {
  
//! Set up function as defined by the unit test implementor
void setUp(Os::Test::Directory::Tester* tester);

//! Tear down function as defined by the unit test implementor
void tearDown(Os::Test::Directory::Tester* tester);

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
