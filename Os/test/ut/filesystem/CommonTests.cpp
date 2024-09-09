// ======================================================================
// \title Os/test/ut/filesystem/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/filesystem/CommonTests.hpp"

#include <STest/Pick/Pick.hpp>


// Initialize static members
std::vector<std::string> Os::Test::FileSystem::Tester::m_test_dirs;
std::vector<std::string> Os::Test::FileSystem::Tester::m_test_files;
Os::Test::FileSystem::Tester::MockDirectory Os::Test::FileSystem::Tester::m_testdir_root("filesystem_test_directory");

std::unique_ptr<Os::Test::FileSystem::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::FileSystem::Tester>(new Os::Test::FileSystem::Tester());
}

Functionality::Functionality() : tester(get_tester_implementation()) {}

// TODO: complete SetUp
void Functionality::SetUp() {
    tester->m_testdir_root = Os::Test::FileSystem::Tester::MockDirectory("filesystem_test_directory");
    Os::FileSystem::createDirectory("filesystem_test_directory");
    tester->m_testdir_root.add_file("test_file1", "abc");
    tester->m_testdir_root.add_file("test_file2", "xyz");
    tester->m_testdir_root.add_directory("sub_dir1");
    tester->m_testdir_root.directories[0].add_file("test_file3", "123");
    tester->m_testdir_root.add_directory("sub_dir2");

    // tester->m_test_dirs.push_back("filesystem_test_dir");
    // tester->m_test_dirs.push_back("filesystem_test_dir/sub_dir");
    // tester->m_test_files.push_back("filesystem_test_dir/test_file1");
    // tester->m_test_files.push_back("filesystem_test_dir/sub_dir/test_file2");

    // Create directory
    // tester->m_testdir_root = Os::Test::FileSystem::Tester::FileSystemTestNode("filesystem_test_dir", true);
    // Os::Directory dir;
    // dir.open(tester->m_test_dirs[0].c_str(), Os::Directory::OpenMode::CREATE);
    // dir.close();
    // dir.open(tester->m_test_dirs[1].c_str(), Os::Directory::OpenMode::CREATE);
    // dir.close();

    // FwSignedSizeType size = 3;
    // U8 buffer1[3] = {'a', 'b', 'c'};
    // U8 buffer2[3] = {'x', 'y', 'z'};

    // Os::File file;
    // file.open(tester->m_test_files[0].c_str(), Os::File::OPEN_WRITE);
    // file.write(buffer1, size);
    // file.close();
    // file.open(tester->m_test_files[1].c_str(), Os::File::OPEN_WRITE);
    // file.write(buffer2, size);
    // file.close();

    // tester->m_testdir_root.add_child_file("filesystem_test_file");

    std::cout << "Setup complete" << std::endl;

}

void Functionality::TearDown() {
    // TODO: delete all files and directories without relying on Os::FileSystem
    // Os::FileSystem::removeFile("filesystem_test_directory/filesystem_test_file");
    // Os::FileSystem::removeDirectory("filesystem_test_directory");
    // Os::Directory dir;
    // Os::File file;

    for (auto filename : tester->m_test_files) {
        Os::FileSystem::removeFile(filename.c_str());
    }
    for (auto it = tester->m_test_dirs.rbegin(); it != tester->m_test_dirs.rend(); ++it) {
        Os::FileSystem::removeDirectory(it->c_str());
    }

    // tester->m_testdir_root.recurse_remove();
    // std::cout << "Tear down complete" << std::endl;
}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------

// Existing paths exists, non-existing paths do not exist
TEST_F(Functionality, Exists) {
    Os::Test::FileSystem::Tester::DirectoryExists dir_exist_rule;
    Os::Test::FileSystem::Tester::FileExists file_exist_rule;
    Os::Test::FileSystem::Tester::PathNotExists no_exist_rule;
    dir_exist_rule.apply(*tester);
    file_exist_rule.apply(*tester);
    no_exist_rule.apply(*tester);
}

// RemoveFile removes a file
TEST_F(Functionality, RemoveFile) {
    Os::Test::FileSystem::Tester::RemoveFile rm_file_rule;
    rm_file_rule.apply(*tester);
}

// RemoveDirectory removes a directory
TEST_F(Functionality, RemoveDirectory) {
    Os::Test::FileSystem::Tester::RemoveDirectory rm_dir_rule;
    rm_dir_rule.apply(*tester);
}

// TouchFile touches a file
TEST_F(Functionality, TouchFile) {
    Os::Test::FileSystem::Tester::TouchFile touch_rule;
    touch_rule.apply(*tester);
}

// CreateDirectory
TEST_F(Functionality, CreateDirectory) {
    Os::Test::FileSystem::Tester::CreateDirectory create_dir_rule;
    create_dir_rule.apply(*tester);
}

// MoveFile 
TEST_F(Functionality, MoveFile) {
    Os::Test::FileSystem::Tester::MoveFile move_rule;
    move_rule.apply(*tester);
}

// CopyFile 
TEST_F(Functionality, CopyFile) {
    Os::Test::FileSystem::Tester::CopyFile move_rule;
    move_rule.apply(*tester);
}

// GetFreeSpace 
TEST_F(Functionality, GetFreeSpace) {
    Os::Test::FileSystem::Tester::GetFreeSpace free_space_rule;
    free_space_rule.apply(*tester);
}
// Change working directory and test that file exists within it


