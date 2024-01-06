// ======================================================================
// \title Os/Posix/test/ut/PosixFileTests.cpp
// \brief tests for posix implementation for Os::File
// ======================================================================
#include <unistd.h>
#include <gtest/gtest.h>
#include "Os/File.hpp"
#include "Os/Posix/File.hpp"
#include "Os/test/ut/CommonFileTests.hpp"

void test_open_helper(Os::File& file, const char* const path, bool cleanup, Os::File::Mode mode, bool overwrite, Os::File::Status expected) {
    if (access(path, F_OK) != 0) {
        Os::File::Status status = file.open(path, mode, overwrite);
        if (cleanup) {
            ::unlink(path);
        }
        ASSERT_EQ(status, expected);
        ASSERT_NE(file.handle->file_descriptor, -1);
    } else {
        GTEST_SKIP() << "Test file '" << path << "' exists before run. Please remove it.";
    }
}

/*TEST(Nominal, OpenCreate) {
    const char* test_path = "/tmp/fprime_posix_test";
    if (access(test_path, F_OK) != 0) {
        test_create_file(test_path, Os::File::Status::OP_OK, false, true);
    } else {
        GTEST_SKIP() << "Test file exists before run. Cannot test.";
    }
}

TEST(Nominal, OpenCreateOverwrite) {
    const char* test_path = "/tmp/fprime_posix_test";
    if (access(test_path, F_OK) != 0) {
        test_create_file(test_path, Os::File::Status::OP_OK, false, false);
        test_create_file(test_path, Os::File::Status::OP_OK, true, true);
    } else {
        GTEST_SKIP() << "Test file exists before run. Cannot test.";
    }
}

TEST(OffNominal, OpenCreateErrorExists) {
    const char* test_path = "/tmp/fprime_posix_test";
    if (access(test_path, F_OK) != 0) {
        test_create_file(test_path, Os::File::Status::OP_OK, false, false);
        test_create_file(test_path, Os::File::Status::FILE_EXISTS, false, true);
    } else {
        GTEST_SKIP() << "Test file exists before run. Cannot test.";
    }
}*/

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
