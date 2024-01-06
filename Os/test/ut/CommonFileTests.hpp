// ======================================================================
// \title Os/test/ut/CommonFileTests.hpp
// \brief definitions used in common file testing
// ======================================================================
#include <Os/File.hpp>
#ifndef OS_TEST_UT_COMMON_FILE_TESTS_HPP
#define OS_TEST_UT_COMMON_FILE_TESTS_HPP

/**
 * \brief open file for test purposes
 *
 * Implemented by the implementer to help open files for generic tests. Open the given file reference with supplied path
 * and the expected status.
 * \param file: file object to open
 * \param path: path to file to open
 * \param expected: expected status
 */
void test_open_helper(Os::File& file, const char* const path, bool cleanup = true, Os::File::Mode mode = Os::File::Mode::OPEN_CREATE, bool overwrite = false, Os::File::Status expected = Os::File::OP_OK);
#endif //OS_TEST_UT_COMMON_FILE_TESTS_HPP