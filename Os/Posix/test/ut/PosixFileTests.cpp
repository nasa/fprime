// ======================================================================
// \title Os/Posix/test/ut/PosixFileTests.cpp
// \brief tests for posix implementation for Os::File
// ======================================================================
#include <gtest/gtest.h>
#include <unistd.h>
#include <list>
#include "Os/File.hpp"
#include "Os/Posix/File.hpp"
#include "Os/test/ut/file/CommonTests.hpp"
#include "STest/Pick/Pick.hpp"
#include <stdio.h>
namespace Os {
namespace Test {
namespace File {

std::list<std::shared_ptr<const char> > FILES;

static const char BASE_PATH[] = "/tmp";
static const char TEST_FILE[] = "fprime-os-file-test";
/**
 * Check if we can use the file. F_OK file exists, R_OK, W_OK are read and write.
 * @return true if it exists, false otherwise.
 */
bool check_permissions(const char* path, int permission) {
    return ::access(path, permission) == 0;
}

/**
 * Get a filename, randomly if random is true, otherwise use a basic filename.
 * @param random: true if filename should be random, false if predictable
 * @return: filename to use for testing
 */
std::shared_ptr<char> get_test_filename(bool random) {
    const char* filename = TEST_FILE;
    char buffer[PATH_MAX - sizeof(BASE_PATH)];
    // When random, select random characters
    if (random) {
        filename = buffer;
        for (size_t i = 0; i < STest::Pick::lowerUpper(1, sizeof buffer); i++) {
            char selected_character = static_cast<char>(STest::Pick::lowerUpper(12, 127));
            selected_character =
                (selected_character == '/') ? static_cast<char>(selected_character + 1) : selected_character;
            buffer[i] = selected_character;
        }
    }
    // Create a shared pointer wrapping our filename buffer
    std::shared_ptr<char> pointer(new char[PATH_MAX], std::default_delete<char[]>());
    (void)snprintf(pointer.get(), PATH_MAX, "%s/%s", BASE_PATH, filename);
    return pointer;
}

/**
 * Set up for the test ensures that the test can run at all
 */
void setUp(bool requires_io) {
    std::shared_ptr<char> non_random_filename = get_test_filename(false);
    // IO required and test file exists then skip
    if (check_permissions(non_random_filename.get(), F_OK)) {
        GTEST_SKIP() << "Test file exists: " << non_random_filename.get();
    }
    // IO required and cannot read/write to BASE_PATH then skip
    else if (requires_io && not check_permissions(BASE_PATH, R_OK & W_OK)) {
        GTEST_SKIP() << "Cannot read/write in directory: " << BASE_PATH;
    }
}

/**
 * Tear down for the tests cleans up the test file used
 */
void tearDown() {
    // Ensure the test files are removed only when the test was run
    for (const auto& val : FILES) {
        if (check_permissions(val.get(), F_OK)) {
            ::unlink(val.get());
        }
    }
    FILES.clear();
}

class PosixTester : public Tester {
    /**
     * Check if the test file exists.
     * @return true if it exists, false otherwise.
     */
    bool exists(const char* filename) const override {
        bool exits =  check_permissions(filename, F_OK);
        return exits;
    }

    /**
     * Get a filename, randomly if random is true, otherwise use a basic filename.
     * @param random: true if filename should be random, false if predictable
     * @return: filename to use for testing
     */
    std::shared_ptr<char> get_filename(bool random) const override {
        std::shared_ptr<char> filename = get_test_filename(random);
        FILES.push_back(filename);
        return filename;
    }

    /**
     * Reports that a file may be opens.
     * @return: true if a file may yet be created, false otherwise
     */
    bool can_create_file() const override { return true; }

    /**
     * Size of given filename.
     * @return: true if a file may yet be created, false otherwise
     */
    FwSizeType size(const char* filename) const override {
        struct stat status;
        EXPECT_EQ(stat(filename, &status), 0);
        return status.st_size;
    }
};

std::unique_ptr<Os::Test::File::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::File::Tester>(new Os::Test::File::PosixTester());
}

}  // namespace File
}  // namespace Test
}  // namespace Os

class Nominal : public Functionality {};
class OffNominal : public Functionality {};

/**
 * Helper function for opening files and asserting that the basic posix implementation works.
 *
 * In the posix implementation of a test, the file must have the expected status and ensure the file descriptor is not
 * set to -1 when the open call succeeds.
 * @param file: file to open
 * @param mode: mode to use, forwarded to `file.open`
 * @param overwrite: overwrite (true/false), forwarded to `file.open`
 * @param expected: expected return status of `open` call
 */
/*void test_open_helper(Os::File& file, Os::File::Mode mode, bool overwrite, Os::File::Status expected) {
    Os::File::Status status = file.open(TEST_FILE_PATH, mode, overwrite);
    ASSERT_EQ(status, expected);
    if (Os::File::Status::OP_OK == status) {
        ASSERT_NE(file.handle->file_descriptor, -1);
    }
}*/
/*
TEST_F(Nominal, OpenCreate) {
    // Create file to test and wrap-it in a cleaner
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
}

TEST_F(Nominal, OpenCreateOverwrite) {
    // Create file to test and wrap-it in a cleaner
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, true, Os::File::Status::OP_OK);
}

TEST_F(OffNominal, OpenCreateErrorExists) {
    // Create file to test and wrap-it in a cleaner
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::FILE_EXISTS);
}
*/
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
