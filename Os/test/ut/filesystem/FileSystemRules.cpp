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

bool check_disk_file_contents(std::string path, std::string contents) {
    Os::File file;
    file.open(path.c_str(), Os::File::OPEN_READ);
    if (contents.size() == 0) {
        FwSignedSizeType bytesRead = 1;
        U8 buffer[1];
        file.read(buffer, bytesRead);
        return bytesRead == 0;
    } else {
        U8 buffer[contents.size()];
        FwSignedSizeType bytesRead = contents.size();
        file.read(buffer, bytesRead);
        return memcmp(buffer, contents.c_str(), contents.size()) == 0;
    }
}

bool check_files_have_same_content(std::string path1, std::string path2) {
    Os::File file1, file2;
    file1.open(path1.c_str(), Os::File::OPEN_READ);
    file2.open(path2.c_str(), Os::File::OPEN_READ);

    const FwSignedSizeType chunk_size = 128;
    U8 buffer1[chunk_size], buffer2[chunk_size];
    FwSignedSizeType bytesRead1 = chunk_size, bytesRead2 = chunk_size;

    while (true) {
        file1.read(buffer1, bytesRead1);
        file2.read(buffer2, bytesRead2);
        if (bytesRead1 != bytesRead2 || memcmp(buffer1, buffer2, bytesRead1) != 0) {
            return false;
        }
        if (bytesRead1 < chunk_size) {
            break; // End of file reached
        }
    }
    return true;
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
    FileTracker& file = state.get_random_file();
    std::string source_path = file.path;
    std::string original_content = file.contents;

    std::string dest_path = state.get_random_new_filepath();
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));
    Os::FileSystem::Status status;
    status = Os::FileSystem::getSingleton().moveFile(source_path.c_str(), dest_path.c_str());
    state.move_file2(file, dest_path);
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
    return true;
}

void Os::Test::FileSystem::Tester::CopyFile::action(Os::Test::FileSystem::Tester &state) {
    Os::FileSystem::Status status;

    FileTracker& source = state.get_random_file();
    std::string source_path = source.path;
    std::string dest_path = state.get_random_new_filepath();

    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    ASSERT_FALSE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));
    status = Os::FileSystem::getSingleton().copyFile(source_path.c_str(), dest_path.c_str());
    state.copy_file2(source, dest_path);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to move file";
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));

    // Compare contents of source and dest on disk
    ASSERT_TRUE(check_files_have_same_content(source_path, dest_path));
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
    FileTracker& source = state.get_random_file();
    std::string source_path = source.path;
    FileTracker& dest = state.get_random_file();
    std::string dest_path = dest.path;

    bool createMissingDest = false;
    // TODO: AH! current appendFile implementation does not allow source and dest to be the same
    // should it be fixed? or assert instead?
    if (source_path == dest_path) {
        // dest = FileTracker(state.get_random_new_filepath(), "");
        // dest.write_on_disk();
        // createMissingDest = true;
        return; // skip this test for now
    } else {
        ASSERT_TRUE(Os::FileSystem::getSingleton().exists(dest_path.c_str()));
    }
    ASSERT_TRUE(Os::FileSystem::getSingleton().exists(source_path.c_str()));
    status = Os::FileSystem::getSingleton().appendFile(source_path.c_str(), dest_path.c_str(), createMissingDest);
    state.append_file2(source, dest, createMissingDest);
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to append file";
    // Compare contents of dest on disk with expected contents
    ASSERT_TRUE(check_disk_file_contents(dest_path, dest.contents));
    ASSERT_TRUE(state.validate_contents_on_disk(dest));
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
    FileTracker file = state.get_random_file();
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
    FwSizeType totalBytes = 0, freeBytes = 0;
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
    std::string other_dir = state.get_random_directory().path;
    status = Os::FileSystem::getSingleton().changeWorkingDirectory(other_dir.c_str());
    ASSERT_EQ(status, Os::FileSystem::Status::OP_OK) << "Failed to change working directory";
    // TODO: cd back to original directory
}

