// ======================================================================
// \title Os/Stub/test/ut/StubFileTests.cpp
// \brief tests using stub implementation for Os::File interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/File.hpp"
#include "Os/Stub/File.hpp"
#include "Os/test/ut/CommonFileTests.hpp"

const char* TEST_FILE_PATH = "/does/not/matter/in/stubbed/tests";
/**
 * Set up for the test ensures that the test can run at all
 */
void setUp(bool requires_io) {
    Os::Stub::file_set_next_status(Os::File::Status::OP_OK);
    if (requires_io) {
        GTEST_SKIP() << "Skipping tests dependent on functional io";
    }
}

/**
 * Tear down for the tests cleans up the test file used
 */
void tearDown() {}

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
void test_open_helper(Os::File& file, Os::File::Mode mode, bool overwrite, Os::File::Status expected) {
    Os::Stub::file_set_next_status(expected);
    Os::File::Status status = file.open(TEST_FILE_PATH, mode, overwrite);
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::OPEN_FN);
    ASSERT_EQ(status, expected);
    ASSERT_NE(file.handle->state, ((Os::File::Status::OP_OK == expected) ? Os::FileHandle::State::FAILED : Os::FileHandle::State::OPENED));
    ASSERT_EQ(file.isOpen(), Os::File::Status::OP_OK == expected);
}

// Ensure that Os::File properly routes constructor calls to the `constructInternal` function.
TEST(Interface, Construction) {
    Os::File file;
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::CONSTRUCT_FN);
}

// Ensure that Os::File properly routes destructor calls to the `destructInternal` function.
TEST(Interface, Destruction) {
    delete (new Os::File);
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::DESTRUCT_FN);
}

// Ensure that Os::File properly routes close calls to the `closeInternal` function before calling destruction.
TEST(Interface, DestructionWithClose) {
    auto file = new Os::File;
    // Replacing the handle with one we control for testing purposes.
    Os::FileHandle test_handle = *file->handle;
    file->handle = &test_handle;

    test_open_helper(*file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
    delete file;
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::DESTRUCT_FN);
    ASSERT_EQ(test_handle.state, Os::FileHandle::State::CLOSED);
}

// Ensure that Os::File properly routes open calls to the `openInternal` function.
TEST(Interface, OpenDefault) {
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
}

// Ensure that Os::File properly routes statuses returned from the `openInternal` function back to the caller.
TEST(Interface, OpenStatusForwarding) {
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::FILE_EXISTS);
}

// Ensure that Os::File properly routes close calls to the `closeInternal` function.
TEST(Interface, Close) {
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
    file.close();
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::CLOSE_FN);
    ASSERT_EQ(file.handle->state, Os::FileHandle::State::CLOSED);
}

// Ensure that Os::File properly routes preallocate calls to the `preallocateInternal` function.
TEST(Interface, Preallocate) {
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
    ASSERT_EQ(file.preallocate(0, 10), Os::File::Status::OP_OK);
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::PREALLOCATE_FN);
    file.close();
}

// Ensure that Os::File properly routes statuses returned from the `preallocateInternal` function back to the caller.
TEST(Interface, PreallocateStatusForwarding) {
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
    Os::Stub::file_set_next_status(Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(file.preallocate(0, 10), Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::PREALLOCATE_FN);
    file.close();
}

// Ensure that Os::File properly routes seek calls to the `seekInternal` function.
TEST(Interface, Seek) {
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
    ASSERT_EQ(file.seek(10, false), Os::File::Status::OP_OK);
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::SEEK_FN);
    ASSERT_EQ(file.seek(10, true), Os::File::Status::OP_OK);
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::SEEK_ABSOLUTE_FN);
    file.close();
}

// Ensure that Os::File properly routes statuses returned from the `seekInternal` function back to the caller.
TEST(Interface, SeekStatusForwarding) {
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_CREATE, false, Os::File::Status::OP_OK);
    Os::Stub::file_set_next_status(Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(file.seek(10, false), Os::File::Status::OTHER_ERROR);
    file.close();
}

// Ensure that Os::File properly routes flush calls to the `flushInternal` function.
TEST(Interface, Flush) {
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_WRITE, false, Os::File::Status::OP_OK);
    ASSERT_EQ(file.flush(), Os::File::Status::OP_OK);
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::FLUSH_FN);
    file.close();
}

// Ensure that Os::File properly routes statuses returned from the `flushInternal` function back to the caller.
TEST(Interface, FlushStatusForwarding) {
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_WRITE, false, Os::File::Status::OP_OK);
    Os::Stub::file_set_next_status(Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(file.flush(), Os::File::Status::OTHER_ERROR);
    file.close();
}

// Ensure that Os::File properly routes flush calls to the `flushInternal` function.
TEST(Interface, Read) {
    U8 byte = 0;
    FwSizeType size = 1;
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_READ, false, Os::File::Status::OP_OK);
    ASSERT_EQ(file.read(&byte, size, false), Os::File::Status::OP_OK);
    ASSERT_EQ(Os::Stub::file_get_last_call(), Os::Stub::LastFunctionCalled::READ_FN);
    file.close();
}

// Ensure that Os::File properly routes statuses returned from the `flushInternal` function back to the caller.
TEST(Interface, ReadStatusForwarding) {
    U8 byte = 0;
    FwSizeType size = 1;
    Os::File file;
    test_open_helper(file, Os::File::Mode::OPEN_READ, false, Os::File::Status::OP_OK);
    Os::Stub::file_set_next_status(Os::File::Status::OTHER_ERROR);
    ASSERT_EQ(file.read(&byte, size, false), Os::File::Status::OTHER_ERROR);
    file.close();
}



int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
