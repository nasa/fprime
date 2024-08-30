// ======================================================================
// \title Os/test/ut/directory/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/directory/CommonTests.hpp"

#include <Os/FileSystem.hpp> // for setup
#include "STest/Pick/Pick.hpp"

// ----------------------------------------------------------------------
// Test Fixture
// ----------------------------------------------------------------------

// MAX_FILES_PER_DIRECTORY is intentionally low to have a high probability of having an empty directory
static const std::string FILENAME_PREFIX = "test_file_";
static const std::string TEST_DIRECTORY_PATH = "./test_directory";

std::unique_ptr<Os::Test::Directory::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::Directory::Tester>(new Os::Test::Directory::Tester());
}

Functionality::Functionality() : tester(get_tester_implementation()) {}

// Create a directory with a random number of files in it
void Functionality::SetUp() {
    tester->m_path = TEST_DIRECTORY_PATH;
    Os::FileSystem::createDirectory(tester->m_path.c_str());
    // Files are named test_file_0, test_file_1, ...
    std::vector<std::string> filenames;
    // fileNumbers can be 0 (empty directory)
    FwSizeType fileNumbers = STest::Pick::lowerUpper(0, Os::Test::Directory::Tester::MAX_FILES_PER_DIRECTORY);
    for (FwSizeType i = 0; i < fileNumbers; i++) {
        tester->m_filenames.push_back(FILENAME_PREFIX + std::to_string(i));
    }
    for (auto filename : tester->m_filenames) {
        Os::FileSystem::touch((tester->m_path + "/" + filename).c_str());
    }
}

void Functionality::TearDown() {
    // No teardown required
    for (auto filename : tester->m_filenames) {
        Os::FileSystem::removeFile((tester->m_path + "/" + filename).c_str());
    }
    Os::FileSystem::removeDirectory(tester->m_path.c_str());
}

// ----------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------


// Open directory and check it is open
TEST_F(Functionality, OpenIsOpen) {
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::IsOpen is_open_rule;
    open_rule.apply(*tester);
    is_open_rule.apply(*tester);
}

// Do not open directory and check it is not open
TEST_F(Functionality, IsNotOpen) {
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::IsNotOpen is_not_open_rule;
    is_not_open_rule.apply(*tester);
    open_rule.apply(*tester);
    close_rule.apply(*tester);
    is_not_open_rule.apply(*tester);
}

// Read file from directory
TEST_F(Functionality, ReadOneFile) {
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::ReadOneFile read_one_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::IsOpen is_open_rule;
    open_rule.apply(*tester);
    is_open_rule.apply(*tester);
    read_one_rule.apply(*tester);
    close_rule.apply(*tester);
}

// Read file from directory
TEST_F(Functionality, ReadRewindRead) {
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::ReadOneFile read_rule;
    Os::Test::Directory::Tester::Rewind rewind_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::IsOpen is_open_rule;
    open_rule.apply(*tester);
    is_open_rule.apply(*tester);
    read_rule.apply(*tester);
    rewind_rule.apply(*tester);
    read_rule.apply(*tester);
    close_rule.apply(*tester);
}

// Read file from directory
TEST_F(Functionality, GetFileCount) {
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::GetFileCount filecount_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::IsOpen is_open_rule;
    open_rule.apply(*tester);
    is_open_rule.apply(*tester);
    filecount_rule.apply(*tester);
    close_rule.apply(*tester);
}

// Read file from directory
TEST_F(Functionality, ReadAllFiles) {
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::ReadAllFiles read_all_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::IsOpen is_open_rule;
    open_rule.apply(*tester);
    is_open_rule.apply(*tester);
    read_all_rule.apply(*tester);
    close_rule.apply(*tester);
}

TEST_F(Functionality, RandomizedTesting) {
    // Enumerate all rules and construct an instance of each
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::IsOpen is_open_rule;
    Os::Test::Directory::Tester::IsNotOpen is_not_open_rule;
    Os::Test::Directory::Tester::ReadOneFile read_rule;
    Os::Test::Directory::Tester::Rewind rewind_rule;
    Os::Test::Directory::Tester::ReadAllFiles read_all_rule;
    Os::Test::Directory::Tester::GetFileCount filecount_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::Directory::Tester>* rules[] = {
            &open_rule,
            &close_rule,
            &is_open_rule,
            &is_not_open_rule,
            &read_rule,
            &rewind_rule,
            &read_all_rule,
            &filecount_rule 
    };

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::Directory::Tester> random(
            "Random Rules",
            rules,
            FW_NUM_ARRAY_ELEMENTS(rules)
    );

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::Directory::Tester> bounded(
            "Bounded Random Rules Scenario",
            random,
            1000
    );
    // Run!
    const U32 numSteps = bounded.run(*tester);
    printf("Ran %u steps with %zu files in test directory.\n", numSteps, tester->m_filenames.size());

}
