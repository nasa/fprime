// ======================================================================
// \title Os/test/ut/filesystem/FileSystemRules.cpp
// \brief rule implementations for common testing of filesystem
// ======================================================================

#include "RulesHeaders.hpp"
#include "FileSystemRules.hpp"
#include "STest/Pick/Pick.hpp"

// ------------------------------------------------------------------------------------------------------
// Utility functions
// ------------------------------------------------------------------------------------------------------

bool compare_file_contents_on_disk(std::string path1, std::string path2) {
    Os::File file1, file2;
    file1.open(path1.c_str(), Os::File::OPEN_READ);
    file2.open(path2.c_str(), Os::File::OPEN_READ);
    
    const FwSignedSizeType chunk_size = 128;

    FwSignedSizeType file1Size, file2Size;
    file1.size(file1Size);
    file2.size(file2Size);
    if (file1Size != file2Size) {
        return false;
    }
    const FwIndexType loopLimit = file1Size / chunk_size + 2;

    U8 buffer1[chunk_size], buffer2[chunk_size];
    FwSignedSizeType bytesRead1 = chunk_size, bytesRead2 = chunk_size;

    for (FwIndexType i = 0; i < loopLimit; i++) {
        file1.read(buffer1, bytesRead1);
        file2.read(buffer2, bytesRead2);
        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0) {
            return false;
        }
        if (bytesRead1 < chunk_size) {
            return true; // End of file reached
        }
    }
    return false;
}

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
    return state.m_test_files.size() > 0;
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
    return state.m_test_files.size() > 0;
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
    std::string new_filename = state.new_random_filepath();
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
    std::string dirpath = state.get_random_directory().path + "/" + state.get_new_dirname();
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
// Rule:  RenameFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::RenameFile::RenameFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("RenameFile") {}

bool Os::Test::FileSystem::Tester::RenameFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return state.m_test_files.size() > 0;
}

void Os::Test::FileSystem::Tester::RenameFile::action(Os::Test::FileSystem::Tester &state) {
    TestFile& file = state.get_random_file();
    std::string source_path = file.path;
    std::string original_content = file.contents;

    std::string dest_path = state.new_random_filepath();
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));
    Os::FileSystem::Status status;
    status = Os::FileSystem::getSingleton().rename(source_path.c_str(), dest_path.c_str());
    state.move_file(file, dest_path);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to rename file";
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));

    // Assert file contents on disk
    ASSERT_TRUE(state.validate_contents_on_disk(file));
}

// ------------------------------------------------------------------------------------------------------
// Rule:  MoveFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::MoveFile::MoveFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("MoveFile") {}

bool Os::Test::FileSystem::Tester::MoveFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return state.m_test_files.size() > 0;
}

void Os::Test::FileSystem::Tester::MoveFile::action(Os::Test::FileSystem::Tester &state) {
    TestFile& file = state.get_random_file();
    std::string source_path = file.path;
    std::string original_content = file.contents;

    std::string dest_path = state.new_random_filepath();
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));
    Os::FileSystem::Status status;
    status = Os::FileSystem::getSingleton().moveFile(source_path.c_str(), dest_path.c_str());
    state.move_file(file, dest_path);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to move file";
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));

    // Assert file contents on disk
    ASSERT_TRUE(state.validate_contents_on_disk(file));
}

// ------------------------------------------------------------------------------------------------------
// Rule:  CopyFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::CopyFile::CopyFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("CopyFile") {}

bool Os::Test::FileSystem::Tester::CopyFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return state.m_test_files.size() > 0;
}

void Os::Test::FileSystem::Tester::CopyFile::action(Os::Test::FileSystem::Tester &state) {
    Os::FileSystem::Status status;

    TestFile& source = state.get_random_file();
    std::string source_path = source.path;
    std::string dest_path = state.new_random_filepath();

    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));
    status = Os::FileSystem::getSingleton().copyFile(source_path.c_str(), dest_path.c_str());
    state.copy_file(source, dest_path);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to move file";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));

    // Compare contents of source and dest on disk
    ASSERT_TRUE(compare_file_contents_on_disk(source_path, dest_path));
}

// ------------------------------------------------------------------------------------------------------
// Rule:  AppendFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::AppendFile::AppendFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("AppendFile") {}

bool Os::Test::FileSystem::Tester::AppendFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return state.m_test_files.size() > 0;
}

void Os::Test::FileSystem::Tester::AppendFile::action(Os::Test::FileSystem::Tester &state) {
    Os::FileSystem::Status status;
    TestFile& source = state.get_random_file();
    std::string source_path = source.path;
    TestFile& dest = state.get_random_file();
    std::string dest_path = dest.path;

    bool createMissingDest = false;
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    status = Os::FileSystem::getSingleton().appendFile(source_path.c_str(), dest_path.c_str(), createMissingDest);
    state.append_file(source, dest, createMissingDest);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to append file";
    // Compare contents of dest on disk with expected contents
    ASSERT_TRUE(state.validate_contents_on_disk(dest));
}

// ------------------------------------------------------------------------------------------------------
// Rule:  AppendToNewFile
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::AppendToNewFile::AppendToNewFile() :
    STest::Rule<Os::Test::FileSystem::Tester>("AppendToNewFile") {}

bool Os::Test::FileSystem::Tester::AppendToNewFile::precondition(const Os::Test::FileSystem::Tester &state) {
    return state.m_test_files.size() > 0;
}

void Os::Test::FileSystem::Tester::AppendToNewFile::action(Os::Test::FileSystem::Tester &state) {
    Os::FileSystem::Status status;
    TestFile& source = state.get_random_file();
    std::string source_path = source.path;
    TestFile& dest = state.get_random_file();
    std::string dest_path = dest.path;

    if (source_path == dest_path) {
        return; // skip test - can not remove dest if it is the same as source
    }
    // Set up test state: remove dest file from disk and reset contents in test state
    bool createMissingDest = true;
    Os::FileSystem::getSingleton().removeFile(dest_path.c_str());
    dest.contents = "";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    // Perform append operation
    status = Os::FileSystem::getSingleton().appendFile(source_path.c_str(), dest_path.c_str(), createMissingDest);
    state.append_file(source, dest, createMissingDest);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to append file";
    // Compare contents of dest on disk with expected contents
    ASSERT_TRUE(state.validate_contents_on_disk(dest));
}

// ------------------------------------------------------------------------------------------------------
// Rule:  GetFileSize
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::GetFileSize::GetFileSize() :
    STest::Rule<Os::Test::FileSystem::Tester>("GetFileSize") {}

bool Os::Test::FileSystem::Tester::GetFileSize::precondition(const Os::Test::FileSystem::Tester &state) {
    return state.m_test_files.size() > 0;
}

void Os::Test::FileSystem::Tester::GetFileSize::action(Os::Test::FileSystem::Tester &state) {
    TestFile file = state.get_random_file();
    Os::FileSystem::Status status;
    FwSignedSizeType size;
    status = Os::FileSystem::getSingleton().getFileSize(file.path.c_str(), size);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to get file size";
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
    FwSizeType totalBytes = 0, freeBytes = 0;
    status = Os::FileSystem::getSingleton().getFreeSpace("/", totalBytes, freeBytes);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to run getFreeSpace";
    ASSERT_GT(totalBytes, 0) << "Total bytes should be greater than 0";
    ASSERT_GT(freeBytes, 0) << "Free bytes should be greater than 0";
    ASSERT_GT(totalBytes, freeBytes) << "Total bytes should be greater than free bytes";
}

// ------------------------------------------------------------------------------------------------------
// Rule:  GetSetWorkingDirectory: Test both get and set working directory
// ------------------------------------------------------------------------------------------------------
Os::Test::FileSystem::Tester::GetSetWorkingDirectory::GetSetWorkingDirectory() :
    STest::Rule<Os::Test::FileSystem::Tester>("GetSetWorkingDirectory") {}

bool Os::Test::FileSystem::Tester::GetSetWorkingDirectory::precondition(const Os::Test::FileSystem::Tester &state) {
    return true;
}

void Os::Test::FileSystem::Tester::GetSetWorkingDirectory::action(Os::Test::FileSystem::Tester &state) {
    Os::FileSystem::Status status;
    FwSizeType cwdSize = PATH_MAX;
    char cwdBuffer[cwdSize];

    // Get original working directory
    status = Os::FileSystem::getSingleton().getWorkingDirectory(cwdBuffer, cwdSize);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to get original working directory";
    std::string original_cwd(cwdBuffer);

    // Change working directory
    std::string other_dir = state.get_random_directory().path;
    status = Os::FileSystem::getSingleton().changeWorkingDirectory(other_dir.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to change working directory";
    status = Os::FileSystem::getSingleton().getWorkingDirectory(cwdBuffer, cwdSize);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to change working directory back";
    ASSERT_TRUE(other_dir.compare(cwdBuffer)) << "getWorkingDirectory did not return the expected directory";

    // Change back to original working directory
    // This is done so that this test does not affect the working directory of other tests during random testing
    status = Os::FileSystem::getSingleton().changeWorkingDirectory(original_cwd.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to change working directory back";
}
