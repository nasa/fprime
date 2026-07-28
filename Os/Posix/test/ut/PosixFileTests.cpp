// ======================================================================
// \title Os/Posix/test/ut/PosixFileTests.cpp
// \brief tests for posix implementation for Os::File
// ======================================================================
#include <gtest/gtest.h>
#include <unistd.h>
#include <config/FppConstantsAc.hpp>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include "Os/File.hpp"
#include "Os/Posix/File.hpp"
#include "Os/test/ut/file/CommonTests.hpp"
#include "STest/Pick/Pick.hpp"
namespace Os {
namespace Test {
namespace FileTest {

std::vector<std::shared_ptr<const std::string> > FILES;

static const U32 MAX_FILES = 500;
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
    // FileNameStringSize is the max string length; +1 for null terminator
    char full_buffer[FileNameStringSize + 1];
    // Cap random part so full path (BASE_PATH + '/' + random + '\0') fits within FileNameStringSize.
    // sizeof(BASE_PATH) accounts for strlen(BASE_PATH) + null terminator, which equals the prefix
    // length (strlen(BASE_PATH) + '/') by coincidence. Subtract 1 to account for the null terminator.
    static const size_t MAX_RANDOM_LEN = FileNameStringSize - sizeof(BASE_PATH) - 1;
    char buffer[MAX_RANDOM_LEN + 1];
    // When random, select random characters
    if (random) {
        filename = buffer;
        size_t i = 0;
        for (i = 0; i < STest::Pick::lowerUpper(2, MAX_RANDOM_LEN); i++) {
            char selected_character = static_cast<char>(STest::Pick::lowerUpper(48, 126));
            selected_character =
                (selected_character == '/') ? static_cast<char>(selected_character + 1) : selected_character;
            buffer[i] = selected_character;
        }
        buffer[i] = 0;  // Terminate random string
    }
    (void)snprintf(full_buffer, sizeof(full_buffer), "%s/%s", BASE_PATH, filename);
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
        bool exits = check_permissions(filename.c_str(), F_OK);
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
    bool functional() const override { return true; }
};

std::unique_ptr<Os::Test::FileTest::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::FileTest::Tester>(new Os::Test::FileTest::PosixTester());
}

}  // namespace FileTest
}  // namespace Test
}  // namespace Os

// ======================================================================
// Regression tests for the EINTR retry-condition inversion in
// Os::Posix::PosixFile::read()/write() (Os/Posix/File.cpp).
//
// Prior to the fix, a genuine (non-EINTR) errno such as ENOSPC or EBADF
// took the loop's `continue` branch and busy-retried the identical failing
// syscall, ultimately falling out of the loop with `status` still OP_OK and
// a short/zero byte count -- a false success. Downstream, Svc::FileUplink
// trusts that OP_OK and then FW_ASSERTs on the short count, aborting the
// flight-software process. These tests drive a real (non-EINTR) I/O error
// through the public Os::File API and assert it is surfaced as a non-OP_OK
// status rather than a false success.
// ======================================================================

//! Force a non-EINTR write error (ENOSPC) using /dev/full, which always
//! fails writes with ENOSPC on Linux. This is the direct analogue of the
//! disk-full-during-uplink scenario. Skipped on platforms without /dev/full
//! (e.g. Darwin).
TEST(PosixFileErrorHandling, WriteNoSpaceSurfacesError) {
    if (::access("/dev/full", W_OK) != 0) {
        GTEST_SKIP() << "/dev/full not available on this platform";
    }
    Os::File file;
    if (file.open("/dev/full", Os::File::OPEN_WRITE) != Os::File::Status::OP_OK) {
        GTEST_SKIP() << "Could not open /dev/full for writing";
    }

    U8 buffer[64] = {0};
    FwSizeType size = sizeof(buffer);
    Os::File::Status status = file.write(buffer, size, Os::File::WaitType::NO_WAIT);

    // Before the fix: status == OP_OK with size == 0 (false success).
    EXPECT_NE(status, Os::File::Status::OP_OK);
    EXPECT_EQ(status, Os::File::Status::NO_SPACE);  // ENOSPC maps to NO_SPACE
    file.close();
}

//! Force a non-EINTR write error (EBADF) by closing the descriptor out from
//! under an open Os::File and then writing. Fully deterministic on any POSIX
//! platform, so it exercises the inverted condition on Linux and Darwin alike.
TEST(PosixFileErrorHandling, WriteBadDescriptorSurfacesError) {
    char path_template[] = "/tmp/fprime-os-file-badfd-write-XXXXXX";
    int tmp_fd = ::mkstemp(path_template);
    ASSERT_GE(tmp_fd, 0) << "Failed to create temp file: " << ::strerror(errno);
    ::close(tmp_fd);  // Os::File opens its own descriptor below

    Os::File file;
    ASSERT_EQ(file.open(path_template, Os::File::OPEN_WRITE), Os::File::Status::OP_OK);

    // Close the descriptor Os::File is holding so the next ::write() -> EBADF.
    Os::Posix::File::PosixFileHandle* handle = static_cast<Os::Posix::File::PosixFileHandle*>(file.getHandle());
    ASSERT_GE(handle->m_file_descriptor, 0);
    ::close(handle->m_file_descriptor);

    U8 buffer[16] = {0};
    FwSizeType size = sizeof(buffer);
    Os::File::Status status = file.write(buffer, size, Os::File::WaitType::NO_WAIT);

    // Before the fix: status == OP_OK with size == 0 (false success).
    EXPECT_NE(status, Os::File::Status::OP_OK);
    EXPECT_EQ(status, Os::File::Status::NOT_OPENED);  // EBADF maps to NOT_OPENED
    EXPECT_LT(size, static_cast<FwSizeType>(sizeof(buffer)));

    // Avoid double-closing a possibly-reused descriptor during teardown.
    handle->m_file_descriptor = Os::Posix::File::PosixFileHandle::INVALID_FILE_DESCRIPTOR;
    file.close();
    ::unlink(path_template);
}

//! Symmetric coverage for the read() path: force a non-EINTR read error
//! (EBADF) by closing the descriptor out from under an open Os::File.
TEST(PosixFileErrorHandling, ReadBadDescriptorSurfacesError) {
    char path_template[] = "/tmp/fprime-os-file-badfd-read-XXXXXX";
    int tmp_fd = ::mkstemp(path_template);
    ASSERT_GE(tmp_fd, 0) << "Failed to create temp file: " << ::strerror(errno);
    ::close(tmp_fd);

    Os::File file;
    ASSERT_EQ(file.open(path_template, Os::File::OPEN_READ), Os::File::Status::OP_OK);

    Os::Posix::File::PosixFileHandle* handle = static_cast<Os::Posix::File::PosixFileHandle*>(file.getHandle());
    ASSERT_GE(handle->m_file_descriptor, 0);
    ::close(handle->m_file_descriptor);

    U8 buffer[16] = {0};
    FwSizeType size = sizeof(buffer);
    Os::File::Status status = file.read(buffer, size, Os::File::WaitType::WAIT);

    // Before the fix: status == OP_OK with size == 0 (false success).
    EXPECT_NE(status, Os::File::Status::OP_OK);
    EXPECT_EQ(status, Os::File::Status::NOT_OPENED);  // EBADF maps to NOT_OPENED
    EXPECT_EQ(size, static_cast<FwSizeType>(0));

    handle->m_file_descriptor = Os::Posix::File::PosixFileHandle::INVALID_FILE_DESCRIPTOR;
    file.close();
    ::unlink(path_template);
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
