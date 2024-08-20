// ======================================================================
// \title Os/Stub/test/ut/StubFileSystemTests.cpp
// \brief stub implementation for Os::FileSystemInterface testing
// This ensures the delegation of function calls happens properly
// ======================================================================
#include <gtest/gtest.h>
#include "Os/test/ut/mutex/CommonTests.hpp"
#include "Os/test/ut/mutex/RulesHeaders.hpp"
#include "Os/Stub/test/FileSystem.hpp"

using namespace Os::Stub::FileSystem::Test;


// Basic file tests
class Interface : public ::testing::Test {
public:
    //! Setup function delegating to UT setUp function
    void SetUp() override {
        StaticData::data = StaticData();
    }

    //! Setup function delegating to UT tearDown function
    void TearDown() override {
        StaticData::data = StaticData();
    }
};


// Ensure that Os::FileSystem properly calls the implementation constructor
// TEST_F(Interface, Construction) {

//     ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::CONSTRUCT_FN);
// }

// // Ensure that Os::FileSystem properly calls the implementation destructor
// TEST_F(Interface, Destruction) {
//     delete (new Os::FileSystem);
//     ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::DESTRUCT_FN);
// }

// Ensure that Os::FileSystem properly calls the implementation createDirectory()
TEST_F(Interface, CreateDirectory) {
    Os::FileSystem::createDirectory("test");
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::CREATE_DIR_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation removeDirectory()
TEST_F(Interface, RemoveDirectory) {
    Os::FileSystem::removeDirectory("test");
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::REMOVE_DIR_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}
// Ensure that Os::FileSystem properly calls the implementation readDirectory()
TEST_F(Interface, ReadDirectory) {
    U32 numFiles;
    Os::FileSystem::readDirectory("test", 0, nullptr, numFiles);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::READ_DIR_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation removeFile()
TEST_F(Interface, RemoveFile) {
    Os::FileSystem::removeFile("file");
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::REMOVE_FILE_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation moveFile()
TEST_F(Interface, MoveFile) {
    Os::FileSystem::moveFile("source", "destination");
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::MOVE_FILE_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation copyFile()
TEST_F(Interface, CopyFile) {
    Os::FileSystem::copyFile("source", "destination");
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::COPY_FILE_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation appendFile()
TEST_F(Interface, AppendFile) {
    Os::FileSystem::appendFile("file", "content");
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::APPEND_FILE_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation getFileSize()
TEST_F(Interface, GetFileSize) {
    FwSignedSizeType size;
    Os::FileSystem::getFileSize("file", size);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::GET_SIZE_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation getFileCount()
TEST_F(Interface, GetFileCount) {
    U32 fileCount;
    Os::FileSystem::getFileCount("directory", fileCount);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::GET_COUNT_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation changeWorkingDirectory()
TEST_F(Interface, ChangeWorkingDirectory) {
    Os::FileSystem::changeWorkingDirectory("directory");
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::CHANGE_CWD_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation getFreeSpace()
TEST_F(Interface, GetFreeSpace) {
    FwSizeType totalBytes;
    FwSizeType freeBytes;
    Os::FileSystem::getFreeSpace("path", totalBytes, freeBytes);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::GET_FREESPACE_FN);
    ASSERT_EQ(StaticData::data.lastStatus, Os::FileSystem::Status::OP_OK);
}

// Ensure that Os::FileSystem properly calls the implementation getHandle()
TEST_F(Interface, GetHandle) {
    ASSERT_EQ(Os::FileSystem::getSingleton().getHandle(), nullptr);
    ASSERT_EQ(StaticData::data.lastCalled, StaticData::LastFn::GET_HANDLE_FN);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    STest::Random::seed();
    return RUN_ALL_TESTS();
}
