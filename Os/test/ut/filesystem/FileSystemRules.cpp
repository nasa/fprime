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
    return state.m_test_dirs.size() > 0; // should always be true in random testing
}

void Os::Test::FileSystem::Tester::DirectoryExists::action(Os::Test::FileSystem::Tester &state) {
    std::string dirPath = state.get_random_directory().path;
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dirPath.c_str())) << "exists() failed for directory";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  FileExists
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::FileExists::FileExists() :
    STest::Rule<Os::Test::FileSystem::Tester>("FileExists") {}

bool Os::Test::FileSystem::Tester::FileExists::precondition(const Os::Test::FileSystem::Tester &state) {
    return state.m_test_files.size() > 0; // should always be true in random testing (see precondition of RemoveFile)
}

void Os::Test::FileSystem::Tester::FileExists::action(Os::Test::FileSystem::Tester &state) {
    std::string filename = state.get_random_file().path;
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
    // Don't remove last file, so as not to end up with no files at all in random testing
    return state.m_test_files.size() > 1;
}

void Os::Test::FileSystem::Tester::RemoveFile::action(Os::Test::FileSystem::Tester &state) {
    std::string filepath = state.get_random_file().path;
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(filepath.c_str()));
    Os::FileSystem::Status status = Os::FileSystem::getSingleton().removeFile(filepath.c_str());
    state.remove_file(filepath);
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
    std::string new_filename = state.get_random_new_filepath();
    Os::FileSystem::Status status;
    status = Os::FileSystem::getSingleton().touch(new_filename.c_str());
    state.touch_file(new_filename);
    // Check that the file was created
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to touch file";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(new_filename.c_str())) << "exists() failed for touched file";
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
    std::string dirpath = state.get_random_directory().path + "/" + state.get_new_dirname(); // TODO: rename with get_new_filename
    Os::FileSystem::Status status;
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dirpath.c_str()));
    status = Os::FileSystem::getSingleton().createDirectory(dirpath.c_str());
    state.create_directory(dirpath);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to create test directory";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dirpath.c_str())) << "exists() should return true for created directory";
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
    // We create a new directory to be removed because we need to ensure that the directory is empty
    // and this is the simplest way to do so. CreateDirectory is already tested above.
    std::string dirpath = state.get_random_directory().path + "/to_be_removed";
    Os::FileSystem::Status status;
    status = Os::FileSystem::getSingleton().createDirectory(dirpath.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to create test directory";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dirpath.c_str())) << "exists() failed for created test directory";
    status = Os::FileSystem::getSingleton().removeDirectory(dirpath.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to remove test directory";
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dirpath.c_str())) << "exists() should return false for removed directory";
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
    std::string source = state.get_random_file().path;
    std::string dest = state.get_random_new_filepath();
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source.c_str()));
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dest.c_str()));
    Os::FileSystem::Status status;
    status = Os::FileSystem::getSingleton().moveFile(source.c_str(), dest.c_str());
    state.move_file(source, dest);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to move file";
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(source.c_str()));
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest.c_str()));
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

    std::string source = state.get_random_file().path;
    std::string dest = state.get_random_new_filepath();

    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source.c_str()));
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dest.c_str()));
    status = Os::FileSystem::getSingleton().copyFile(source.c_str(), dest.c_str());
    state.copy_file(source, dest);
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

    // state.track_file_for_cleanup(dest);
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
    Os::FileSystem::Status status;
    std::string source = state.get_random_file().path;
    std::string dest = state.get_random_file().path;
    bool createMissingDest = false;
    // TODO: AH! there's an issue when source and dest are the same because of appendFile implementation
    // workaround is to not allow source and dest to be the same - should it be fixed?
    if (source == dest) {
        dest = state.get_random_new_filepath();
        createMissingDest = true;
    } else {
        ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest.c_str()));
    }
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source.c_str()));
    status = Os::FileSystem::getSingleton().appendFile(source.c_str(), dest.c_str(), createMissingDest);
    state.append_file(source, dest, createMissingDest);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to append file";
    // TODO: Compare contents of source and dest
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
    MockFile file = state.get_random_file();
    Os::FileSystem::Status status;
    FwSignedSizeType size;
    status = Os::FileSystem::getSingleton().getFileSize(file.path.c_str(), size);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to get file size";
    // TODO: make sure .size() is ok - might want to rework strings into U8 buffers
    ASSERT_EQ(size, file.contents.size()) << "File size should match contents size";
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
    std::string other_dir = get_random_directory().path;
    status = Os::FileSystem::getSingleton().changeWorkingDirectory(other_dir.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to change working directory";
}

