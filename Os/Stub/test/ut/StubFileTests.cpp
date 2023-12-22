// ======================================================================
// \title Os/Posix/test/ut/PosixFileTests.cpp
// \brief tests for posix implementation for Os::File
// ======================================================================
#include <gtest/gtest.h>
#include "Os/File.hpp"
#include "Os/Stub/File.hpp"



TEST(Nominal, OpenDefault) {
    Os::File file;
    Os::File::Status status = file.open("/does/not/matter", Os::File::Mode::OPEN_CREATE, true);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
}

TEST(Nominal, OpenSetStatus) {
    const Os::File::Status expected = Os::File::Status::FILE_EXISTS;
    Os::Stub::file_set_next_status(expected);
    Os::File file;
    Os::File::Status status = file.open("/does/not/matter", Os::File::Mode::OPEN_CREATE);
    ASSERT_EQ(status, expected);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
