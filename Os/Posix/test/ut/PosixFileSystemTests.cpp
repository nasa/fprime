// ======================================================================
// \title Os/Posix/test/ut/PosixFileSystemTests.cpp
// \brief tests for posix implementation for Os::FileSystem
// ======================================================================
#include <gtest/gtest.h>
#include "Fw/Types/String.hpp"
#include "Os/Posix/Task.hpp"
#include "Os/test/ut/filesystem/CommonTests.hpp"
#include "Os/test/ut/filesystem/RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"
#include "STest/Scenario/Scenario.hpp"

// ----------------------------------------------------------------------
// Posix Test Cases
// ----------------------------------------------------------------------

// POSIX-specific test class
class PosixFileSystemTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Create a test directory
        m_test_dir = "posix_test_dir";
        Os::FileSystem::createDirectory(m_test_dir.c_str());
    }

    void TearDown() override {
        // Clean up test directory
        Os::FileSystem::removeDirectory(m_test_dir.c_str());
    }

    std::string m_test_dir;
};

// Test POSIX-specific path types (FIFO, symlinks, etc.)
TEST_F(PosixFileSystemTest, DetectSpecialPathTypes) {
    // Test directory detection
    Os::FileSystem::PathType dirType = Os::FileSystem::getSingleton().getPathType(m_test_dir.c_str());
    ASSERT_EQ(dirType, Os::FileSystem::PathType::DIRECTORY) << "Failed to detect directory: " << m_test_dir;

    // Create a test file
    std::string testFile = m_test_dir + "/test_file.txt";
    ASSERT_EQ(Os::FileSystem::touch(testFile.c_str()), Os::FileSystem::Status::OP_OK) << "Failed to create test file";

    // Test file detection
    Os::FileSystem::PathType fileType = Os::FileSystem::getSingleton().getPathType(testFile.c_str());
    ASSERT_EQ(fileType, Os::FileSystem::PathType::FILE) << "Failed to detect file: " << testFile;

    // Test nonexistent path
    std::string nonExistentPath = "non_existent_path";
    Os::FileSystem::PathType nonExistentType = Os::FileSystem::getSingleton().getPathType(nonExistentPath.c_str());
    ASSERT_EQ(nonExistentType, Os::FileSystem::PathType::NOT_EXIST) << "Failed to detect nonexistent path";

    // Clean up test file
    ASSERT_EQ(Os::FileSystem::removeFile(testFile.c_str()), Os::FileSystem::Status::OP_OK)
        << "Failed to remove test file";

    // Test FIFO (named pipe)
    std::string fifoPath = m_test_dir + "/test_fifo";
    ASSERT_EQ(mkfifo(fifoPath.c_str(), 0666), 0) << "Failed to create FIFO";

    Os::FileSystem::PathType fifoType = Os::FileSystem::getPathType(fifoPath.c_str());
    ASSERT_EQ(fifoType, Os::FileSystem::PathType::OTHER) << "Failed to detect FIFO as OTHER type";

    // Clean up FIFO
    ASSERT_EQ(unlink(fifoPath.c_str()), 0) << "Failed to remove FIFO";

    // Test directory vs file with similar names
    std::string subdirPath = m_test_dir + "/test_subdir";
    ASSERT_EQ(Os::FileSystem::createDirectory(subdirPath.c_str()), Os::FileSystem::Status::OP_OK)
        << "Failed to create test subdirectory";

    // Create a file with the same name as the directory plus extension
    ASSERT_EQ(Os::FileSystem::touch((subdirPath + ".txt").c_str()), Os::FileSystem::Status::OP_OK)
        << "Failed to create test file";

    // Verify directory detection
    ASSERT_EQ(Os::FileSystem::getPathType(subdirPath.c_str()), Os::FileSystem::PathType::DIRECTORY)
        << "Failed to detect directory with file in same path";

    // Verify file detection
    ASSERT_EQ(Os::FileSystem::getPathType((subdirPath + ".txt").c_str()), Os::FileSystem::PathType::FILE)
        << "Failed to detect file with directory in same path";

    // Clean up subdirectory
    ASSERT_EQ(Os::FileSystem::removeDirectory(subdirPath.c_str()), Os::FileSystem::Status::OP_OK)
        << "Failed to remove test directory";
}

int main(int argc, char** argv) {
    STest::Random::seed();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
