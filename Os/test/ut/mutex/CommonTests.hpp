// ======================================================================
// \title Os/test/ut/Mutex/CommonTests.hpp
// \brief definitions used in common Mutex testing
// ======================================================================
#include <Os/Mutex.hpp>
#include <Os/test/ut/Mutex/RulesHeaders.hpp>

#ifndef OS_TEST_UT_COMMON_MUTEX_TESTS_HPP
#define OS_TEST_UT_COMMON_MUTEX_TESTS_HPP
namespace Os {
namespace Test {
namespace Mutex {

//! Set up function as defined by the unit test implementor
void setUp(bool requires_io  //!< Does this test require functional io devices
);

//! Tear down function as defined by the unit test implementor
void tearDown();

}  // namespace Mutex
}  // namespace Test
}  // namespace Os


#endif  // OS_TEST_UT_COMMON_MUTEX_TESTS_HPP
