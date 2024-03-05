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
#include <cstdio>
#include <csignal>
namespace Os {
namespace Test {
namespace File {

std::vector<std::shared_ptr<const std::string> > FILES;

static const U32 MAX_FILES = 100;
static const char BASE_PATH[] = "/tmp/fprime";
static const char TEST_FILE[] = "fprime-os-file-test";
//! Check if we can use the file. F_OK file exists, R_OK, W_OK are read and write.
//! \return true if it exists, false otherwise.
//!
bool check_permissions(const char* path, int permission) {
    return ::access(path, permission) == 0;
}

//! Get a filename, randomly if random is true, otherwise use a basic filename.
//! \param random: true if filename should be random, false if predictable
//! \return: filename to use for testing
//!
std::shared_ptr<std::string> get_test_filename(bool random) {
    const char* filename = TEST_FILE;
    char full_buffer[PATH_MAX];
    char buffer[PATH_MAX - sizeof(BASE_PATH)];
    // When random, select random characters
    if (random) {
        filename = buffer;
        size_t i = 0;
        for (i = 0; i < STest::Pick::lowerUpper(1, (sizeof buffer) - 1); i++) {
            char selected_character = static_cast<char>(STest::Pick::lowerUpper(32, 126));
            selected_character =
                (selected_character == '/') ? static_cast<char>(selected_character + 1) : selected_character;
            buffer[i] = selected_character;
        }
        buffer[i] = 0; // Terminate random string
    }
    (void) snprintf(full_buffer, PATH_MAX, "%s/%s", BASE_PATH, filename);
    // Create a shared pointer wrapping our filename buffer
    std::shared_ptr<std::string> pointer(new std::string(full_buffer), std::default_delete<std::string>());
    return pointer;
}

//! Clean-up the files created during this test.
//!
void cleanup(int signal) {
    // Ensure the test files are removed only when the test was run
    for (const auto& val : FILES) {
        if (check_permissions(val->c_str(), F_OK)) {
            ::unlink(val->c_str());
        }
    }
    FILES.clear();
}

//! Set up for the test ensures that the test can run at all
//!
void setUp(bool requires_io) {
    std::shared_ptr<std::string> non_random_filename = get_test_filename(false);
    int result = mkdir(BASE_PATH, 0777);
    // Check that we could make the directory for test files
    if (result != 0 && errno != EEXIST) {
        GTEST_SKIP() << "Cannot make directory for test files: " << strerror(errno);
    }
    // IO required and test file exists then skip
    else if (check_permissions(non_random_filename->c_str(), F_OK)) {
        GTEST_SKIP() << "Test file exists: " << non_random_filename->c_str();
    }
    // IO required and cannot read/write to BASE_PATH then skip
    else if (requires_io && not check_permissions(BASE_PATH, R_OK & W_OK)) {
        GTEST_SKIP() << "Cannot read/write in directory: " << BASE_PATH;
    }
    int signals[] = {SIGQUIT, SIGABRT, SIGTERM, SIGINT, SIGHUP};
    for (unsigned long i = 0; i < FW_NUM_ARRAY_ELEMENTS(signals); i++) {
        // Could not register signal handler
        if (signal(SIGQUIT, cleanup) == SIG_ERR) {
            GTEST_SKIP() << "Cannot register signal handler for cleanup";
        }
    }
}

//! Tear down for the tests cleans up the test file used
//!
void tearDown() {
    cleanup(0);
}

class PosixTester : public Tester {
    //! Check if the test file exists.
    //! \return true if it exists, false otherwise.
    //!
    bool exists(const std::string& filename) const override {
        bool exits =  check_permissions(filename.c_str(), F_OK);
        return exits;
    }

    //! Get a filename, randomly if random is true, otherwise use a basic filename.
    //! \param random: true if filename should be random, false if predictable
    //! \return: filename to use for testing
    //!
    std::shared_ptr<const std::string> get_filename(bool random) const override {
        U32 pick = STest::Pick::lowerUpper(0, MAX_FILES);
        if (random && pick < FILES.size()) {
            return FILES[pick];
        }
        std::shared_ptr<const std::string> filename = get_test_filename(random);
        FILES.push_back(filename);
        return filename;
    }

    //! Posix tester is fully functional
    //! \return true
    //!
    bool functional() const override{
        return true;
    }

};

std::unique_ptr<Os::Test::File::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::File::Tester>(new Os::Test::File::PosixTester());
}

}  // namespace File
}  // namespace Test
}  // namespace Os

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
