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
    using namespace Os::Test::FileSystem;
    const FwSizeType NUMBER_TEST_FILES = 30;

    // Lambda function to generate a random alphanumeric string
    // used to populate the contents of the test files
    // If need to debug, it can be useful to manually set strings
    // of known values in this->m_test_files instead
    auto generate_random_string = []() -> std::string {
        static const char alphanums[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        FwIndexType len = STest::Pick::lowerUpper(1, 4);
        std::string result;
        result.reserve(len);
        for (FwIndexType i = 0; i < len; ++i) {
            result += alphanums[STest::Pick::lowerUpper(0, sizeof(alphanums) - 2)];
        }
        return result;
    };

    // Set up test state - create a directory structure with files
    std::string root_dir = "filesystem_test_directory";
    tester->m_test_dirs = {
        TestDirectory(root_dir),
        TestDirectory(root_dir + "/sub_dir_1"),
        TestDirectory(root_dir + "/sub_dir_2")
    };

    for (FwSizeType i = 0; i < NUMBER_TEST_FILES; ++i) {
        std::string path = root_dir + "/test_file_" + std::to_string(i);
        tester->m_test_files.push_back(TestFile(path, generate_random_string()));
    }

    // All of the above is in the tester state, i.e. in memory
    // The below initializes the filesystem with the same structure, on disk
    tester->write_test_state_to_disk();
}

void Functionality::TearDown() {
    tester->purge_test_state_from_disk();
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

// RenameFile 
TEST_F(Functionality, RenameFile) {
    Os::Test::FileSystem::Tester::RenameFile rename_rule;
    rename_rule.apply(*tester);
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

// AppendToNewFile 
TEST_F(Functionality, AppendToNewFile) {
    Os::Test::FileSystem::Tester::AppendToNewFile append_new_rule;
    append_new_rule.apply(*tester);
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

// // Test both get and set working directory
TEST_F(Functionality, GetSetWorkingDirectory) {
    Os::Test::FileSystem::Tester::GetSetWorkingDirectory change_cwd_rule;
    change_cwd_rule.apply(*tester);
}

// Randomized testing
TEST_F(Functionality, RandomizedTesting) {
    // Enumerate all rules and construct an instance of each
    
    Os::Test::FileSystem::Tester::DirectoryExists directory_exists_rule;
    Os::Test::FileSystem::Tester::FileExists file_exists_rule;
    Os::Test::FileSystem::Tester::PathNotExists not_exists_rule;
    Os::Test::FileSystem::Tester::RemoveFile remove_rule;
    Os::Test::FileSystem::Tester::RemoveDirectory remove_directory_rule;
    Os::Test::FileSystem::Tester::TouchFile touch_rule;
    Os::Test::FileSystem::Tester::CreateDirectory create_directory_rule;
    Os::Test::FileSystem::Tester::MoveFile move_rule;
    Os::Test::FileSystem::Tester::RenameFile rename_rule;
    Os::Test::FileSystem::Tester::CopyFile copyfile_rule;
    Os::Test::FileSystem::Tester::AppendFile append_rule;
    Os::Test::FileSystem::Tester::AppendToNewFile append_new_rule;
    Os::Test::FileSystem::Tester::GetFileSize file_size_rule;
    Os::Test::FileSystem::Tester::GetFreeSpace free_space_rule;
    Os::Test::FileSystem::Tester::GetSetWorkingDirectory cwd_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::FileSystem::Tester>* rules[] = {
            &directory_exists_rule,
            &file_exists_rule,
            &not_exists_rule,
            &remove_rule,
            &remove_directory_rule,
            &touch_rule,
            &create_directory_rule,
            &move_rule,
            &rename_rule,
            &copyfile_rule,
            &append_rule,
            &append_new_rule,
            &file_size_rule,
            &free_space_rule,
            &cwd_rule
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
            1000
    );
    // Run!
    const U32 numSteps = bounded.run(*tester);
    printf("Ran %u steps.\n", numSteps);

}
