// ======================================================================
// \title Os/test/ut/filesystem/FileSystemRules.cpp
// \brief rule implementations for common testing of filesystem
// ======================================================================

#include "RulesHeaders.hpp"
#include "FileSystemRules.hpp"
#include "STest/Pick/Pick.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  DirectoryExists
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::DirectoryExists::DirectoryExists() :
    STest::Rule<Os::Test::FileSystem::Tester>("DirectoryExists") {}

bool Os::Test::FileSystem::Tester::DirectoryExists::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::DirectoryExists::action(Os::Test::FileSystem::Tester &state) {
    std::string dirPath = state.get_random_directory();
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dirPath.c_str())) << "exists() failed for directory";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  FileExists
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::FileExists::FileExists() :
    STest::Rule<Os::Test::FileSystem::Tester>("FileExists") {}

bool Os::Test::FileSystem::Tester::FileExists::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::FileExists::action(Os::Test::FileSystem::Tester &state) {
    std::string filename = state.get_random_file();
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(filename.c_str())) << "exists() failed for file";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  PathNotExists
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::PathNotExists::PathNotExists() :
    STest::Rule<Os::Test::FileSystem::Tester>("PathNotExists") {}

bool Os::Test::FileSystem::Tester::PathNotExists::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::PathNotExists::action(Os::Test::FileSystem::Tester &state) {
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists("does_not_exist"))
                << "exists() failed to return false for non-existent path";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  RemoveFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::RemoveFile::RemoveFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("RemoveFile") {}

bool Os::Test::FileSystem::Tester::RemoveFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::RemoveFile::action(Os::Test::FileSystem::Tester &state) {
    std::string filepath = state.get_random_file();
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(filepath.c_str()));
    Os::FileSystem::Status status = Os::FileSystem::getSingleton().removeFile(filepath.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to remove test file";
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(filepath.c_str())) << "exists() failed for touched file";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  TouchFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::TouchFile::TouchFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("TouchFile") {}

bool Os::Test::FileSystem::Tester::TouchFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::TouchFile::action(Os::Test::FileSystem::Tester &state) {
    std::string new_filename = state.get_random_directory() + "/new_file";
    Os::FileSystem::Status status;
    status = Os::FileSystem::getSingleton().touch(new_filename.c_str());
    // Check that the file was created
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to touch file";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(new_filename.c_str())) << "exists() failed for touched file";

    state.track_file_for_cleanup(new_filename);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  RemoveDirectory
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::RemoveDirectory::RemoveDirectory() :
    STest::Rule<Os::Test::FileSystem::Tester>("RemoveDirectory") {}

bool Os::Test::FileSystem::Tester::RemoveDirectory::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::RemoveDirectory::action(Os::Test::FileSystem::Tester &state) {
    std::string filepath = state.m_test_files[1];
    Os::FileSystem::Status status;
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(filepath.c_str()));
    // Remove the file within (removeFile already tested above)
    status = Os::FileSystem::getSingleton().removeFile(filepath.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to remove test file";
    status = Os::FileSystem::getSingleton().removeDirectory(state.m_test_dirs[1].c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to remove test directory";
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(state.m_test_dirs[1].c_str())) << "exists() should return false for removed directory";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  CreateDirectory
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::CreateDirectory::CreateDirectory() :
    STest::Rule<Os::Test::FileSystem::Tester>("CreateDirectory") {}

bool Os::Test::FileSystem::Tester::CreateDirectory::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::CreateDirectory::action(Os::Test::FileSystem::Tester &state) {
    std::string dirpath = state.get_random_directory() + "/new_directory";
    Os::FileSystem::Status status;
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dirpath.c_str()));
    status = Os::FileSystem::getSingleton().createDirectory(dirpath.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to create test directory";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dirpath.c_str())) << "exists() should return true for created directory";

    state.track_directory_for_cleanup(dirpath);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  MoveFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::MoveFile::MoveFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("MoveFile") {}

bool Os::Test::FileSystem::Tester::MoveFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::MoveFile::action(Os::Test::FileSystem::Tester &state) {
    std::string source = state.get_random_file();
    std::string dest = source + "_moved";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source.c_str()));
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dest.c_str()));
    Os::FileSystem::Status status;
    status = Os::FileSystem::getSingleton().moveFile(source.c_str(), dest.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to move file";
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(source.c_str()));
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest.c_str()));

    state.track_file_for_cleanup(dest);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  CopyFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::CopyFile::CopyFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("CopyFile") {}

bool Os::Test::FileSystem::Tester::CopyFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::CopyFile::action(Os::Test::FileSystem::Tester &state) {
    Os::FileSystem::Status status;

    std::string source = Os::Test::FileSystem::Tester::get_random_file();
    std::string dest = Os::Test::FileSystem::Tester::get_random_directory() + "copiedFile";

    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source.c_str()));
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dest.c_str()));
    status = Os::FileSystem::getSingleton().copyFile(source.c_str(), dest.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to move file";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source.c_str()));
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest.c_str()));
    // Compare contents of source and dest
    // state.assert_file_contents_equal(source, dest);
    Os::File file;
    file.open(source.c_str(), Os::File::OPEN_READ);
    U8 buffer[4];
    FwSignedSizeType bytesRead = 4;
    file.read(buffer, bytesRead);

    state.track_file_for_cleanup(dest);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  AppendFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::AppendFile::AppendFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("AppendFile") {}

bool Os::Test::FileSystem::Tester::AppendFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::AppendFile::action(Os::Test::FileSystem::Tester &state) {

}

// ------------------------------------------------------------------------------------------------------
// Rule:  GetFileSize
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::GetFileSize::GetFileSize() :
    STest::Rule<Os::Test::FileSystem::Tester>("GetFileSize") {}

bool Os::Test::FileSystem::Tester::GetFileSize::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::GetFileSize::action(Os::Test::FileSystem::Tester &state) {

}

// ------------------------------------------------------------------------------------------------------
// Rule:  GetFreeSpace
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::GetFreeSpace::GetFreeSpace() :
    STest::Rule<Os::Test::FileSystem::Tester>("GetFreeSpace") {}

bool Os::Test::FileSystem::Tester::GetFreeSpace::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::GetFreeSpace::action(Os::Test::FileSystem::Tester &state) {
    Os::FileSystem::Status status;
    FwSizeType totalBytes, freeBytes;
    status = Os::FileSystem::getSingleton().getFreeSpace("/", totalBytes, freeBytes);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to run getFreeSpace";
    ASSERT_GT(totalBytes, 0) << "Total bytes should be greater than 0";
    ASSERT_GT(freeBytes, 0) << "Free bytes should be greater than 0";
    ASSERT_GT(totalBytes, freeBytes) << "Total bytes should be greater than free bytes";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ChangeWorkingDirectory
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::ChangeWorkingDirectory::ChangeWorkingDirectory() :
    STest::Rule<Os::Test::FileSystem::Tester>("ChangeWorkingDirectory") {}

bool Os::Test::FileSystem::Tester::ChangeWorkingDirectory::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::ChangeWorkingDirectory::action(Os::Test::FileSystem::Tester &state) {
    Os::FileSystem::Status status;
    std::string other_dir = get_random_directory();
    status = Os::FileSystem::getSingleton().changeWorkingDirectory(other_dir.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to change working directory";
}

