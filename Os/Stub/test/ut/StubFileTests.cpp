// ======================================================================
// \title Os/Stub/test/ut/StubFileTests.cpp
// \brief tests using stub implementation for Os::File interface testing
// ======================================================================
#include <gtest/gtest.h>
#include "Os/File.hpp"
#include "Os/test/ut/file/CommonTests.hpp"


// Ensure that Os::File properly routes constructor calls to the `constructInternal` function.
TEST(Interface, Construction) {
    Os::File file;
}

// Ensure that Os::File properly routes destructor calls to the `destructInternal` function.
TEST(Interface, Destruction) {
    delete (new Os::File);
}

// Ensure that Os::File properly routes open calls to the `openInternal` function.
TEST(Interface, Open) {
    Os::File file;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, true), Os::File::Status::OTHER_ERROR);
}

// Ensure that Os::File properly routes close calls to the `closeInternal` function.
TEST(Interface, Close) {
    Os::File file;
    ASSERT_EQ(file.open("/does/not/matter", Os::File::OPEN_CREATE, true), Os::File::Status::OTHER_ERROR);
    file.close();
}

// Ensure that Os::File properly routes preallocate calls to the `preallocateInternal` function.
TEST(Interface, Preallocate) {
    Os::File file;
    file.mode = Os::File::Mode::OPEN_CREATE;
    ASSERT_EQ(file.preallocate(0, 0), Os::File::Status::OTHER_ERROR);
}

// Ensure that Os::File properly routes seek calls to the `seekInternal` function.
TEST(Interface, Seek) {
    Os::File file;
    file.mode = Os::File::Mode::OPEN_CREATE;
    ASSERT_EQ(file.seek(0, true), Os::File::Status::OTHER_ERROR);
}

// Ensure that Os::File properly routes flush calls to the `flushInternal` function.
TEST(Interface, Flush) {
    Os::File file;
    file.mode = Os::File::Mode::OPEN_CREATE;
    ASSERT_EQ(file.flush(), Os::File::Status::OTHER_ERROR);
}

// Ensure that Os::File properly routes flush calls to the `flushInternal` function.
TEST(Interface, Read) {
    U8 data[10];
    FwSizeType size = 1;
    Os::File file;
    file.mode = Os::File::Mode::OPEN_READ;
    ASSERT_EQ(file.read(data, size, true), Os::File::Status::OTHER_ERROR);
}

// Ensure that Os::File properly routes statuses returned from the `flushInternal` function back to the caller.
TEST(Interface, ReadStatusForwarding) {
    FwSizeType size = 1;
    Os::File file;
    file.mode = Os::File::Mode::OPEN_WRITE;
    ASSERT_EQ(file.write("some-data", size, true), Os::File::Status::OTHER_ERROR);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
