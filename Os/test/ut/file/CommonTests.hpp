// ======================================================================
// \title Os/test/ut/CommonFileTests.hpp
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

class Functionality : public ::testing::Test {
  public:
    //! Setup function delegating to UT setUp function
    virtual void SetUp() override;

    //! Setup function delegating to UT tearDown function
    virtual void TearDown() override;
};
#endif  // OS_TEST_UT_COMMON_FILE_TESTS_HPP