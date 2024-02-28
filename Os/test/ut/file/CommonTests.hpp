// ======================================================================
// \title Os/test/ut/file/CommonFileTests.hpp
// \brief definitions used in common file testing
// ======================================================================
#include <Os/File.hpp>
#include <Os/test/ut/file/RulesHeaders.hpp>

#ifndef OS_TEST_UT_COMMON_FILE_TESTS_HPP
#define OS_TEST_UT_COMMON_FILE_TESTS_HPP
namespace Os {
namespace Test {
namespace File {

//! Set up function as defined by the unit test implementor
void setUp(bool requires_io  //!< Does this test require functional io devices
);

//! Tear down function as defined by the unit test implementor
void tearDown();

}  // namespace File
}  // namespace Test
}  // namespace Os

// Basic file tests
class Functionality : public ::testing::Test {
  public:
    //! Constructor
    Functionality();

    //! Setup function delegating to UT setUp function
    void SetUp() override;

    //! Setup function delegating to UT tearDown function
    void TearDown() override;

    //! Tester/state implementation
    std::unique_ptr<Os::Test::File::Tester> tester;
};

//! Interface testing
class Interface : public Functionality {};

//! Category of tests to check for invalid argument assertions
class InvalidArguments : public Functionality {};

//! Category of tests dependent on functional io
class FunctionalIO : public Functionality {

    //! Specialized setup method used to pass requirement for functional i/o
    void SetUp() override;
};

#endif  // OS_TEST_UT_COMMON_FILE_TESTS_HPP
