// ======================================================================
// \title Os/test/ut/filesystem/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/filesystem/CommonTests.hpp"

#include <STest/Pick/Pick.hpp>



std::unique_ptr<Os::Test::FileSystem::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::FileSystem::Tester>(new Os::Test::FileSystem::Tester());
}

Functionality::Functionality() : tester(get_tester_implementation()) {}

void Functionality::SetUp() {
    tester->m_test_dirs = {
        {"filesystem_test_directory"},
        {"filesystem_test_directory/sub_dir1"},
        {"filesystem_test_directory/sub_dir2"}
    };

    tester->m_test_files = {
        {"filesystem_test_directory/test_file0", "123"},
        {"filesystem_test_directory/test_file1", "abc"},
        {"filesystem_test_directory/test_file2", "xyz"},
        {"filesystem_test_directory/test_file3", "aaaa"},
        {"filesystem_test_directory/test_file4", "dddd"},
        {"filesystem_test_directory/test_file5", "sef"},
        {"filesystem_test_directory/sub_dir1/test_file3", "789"}
    };

    tester->write_test_state_to_disk();
}

void Functionality::TearDown() {
    tester->purge_test_state_from_disk();
    std::cout << "Tear down complete" << std::endl;
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

// AppendFile 
TEST_F(Functionality, AppendFile) {
    Os::Test::FileSystem::Tester::AppendFile append_rule;
    append_rule.apply(*tester);
}

// GetFileSize 
TEST_F(Functionality, GetFileSize) {
    Os::Test::FileSystem::Tester::GetFileSize get_size_rule;
    get_size_rule.apply(*tester);
}

// GetFreeSpace 
TEST_F(Functionality, GetFreeSpace) {
    Os::Test::FileSystem::Tester::GetFreeSpace free_space_rule;
    free_space_rule.apply(*tester);
}

// // ChangeWorkingDirectory 
// TEST_F(Functionality, ChangeWorkingDirectory) {
//     Os::Test::FileSystem::Tester::ChangeWorkingDirectory change_cwd_rule;
//     change_cwd_rule.apply(*tester);
// }

// Randomized testing
TEST_F(Functionality, RandomizedTesting) {
    // Enumerate all rules and construct an instance of each
    
    Os::Test::FileSystem::Tester::DirectoryExists directory_exists_rule;
    Os::Test::FileSystem::Tester::FileExists fileexists_rule;
    Os::Test::FileSystem::Tester::PathNotExists pathnotexists_rule;
    Os::Test::FileSystem::Tester::RemoveFile removefile_rule;
    Os::Test::FileSystem::Tester::RemoveDirectory remove_directory_rule;
    Os::Test::FileSystem::Tester::TouchFile touchfile_rule;
    Os::Test::FileSystem::Tester::CreateDirectory create_directory_rule;
    Os::Test::FileSystem::Tester::MoveFile movefile_rule;
    Os::Test::FileSystem::Tester::CopyFile copyfile_rule;
    Os::Test::FileSystem::Tester::AppendFile append_rule;
    Os::Test::FileSystem::Tester::GetFileSize getfilesize_rule;
    Os::Test::FileSystem::Tester::GetFreeSpace getfreespace_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::FileSystem::Tester>* rules[] = {
            &directory_exists_rule,
            &fileexists_rule,
            &pathnotexists_rule,
            &removefile_rule,
            &remove_directory_rule,
            &touchfile_rule,
            &create_directory_rule,
            &movefile_rule,
            &copyfile_rule,
            &append_rule,
            &getfilesize_rule,
            &getfreespace_rule
    };

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::FileSystem::Tester> random(
            "Random Rules",
            rules,
            FW_NUM_ARRAY_ELEMENTS(rules)
    );

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::FileSystem::Tester> bounded(
            "Bounded Random Rules Scenario",
            random,
            10000
    );
    // Run!
    const U32 numSteps = bounded.run(*tester);
    printf("Ran %u steps.\n", numSteps);

}
