// ======================================================================
// \title Os/test/ut/directory/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/directory/CommonTests.hpp"

#include <Os/FileSystem.hpp> // for setup
#include "STest/Pick/Pick.hpp"


std::unique_ptr<Os::Test::Directory::Tester> get_tester_implementation() {
    return std::unique_ptr<Os::Test::Directory::Tester>(new Os::Test::Directory::Tester());
}

Functionality::Functionality() : tester(get_tester_implementation()) {}


//! Create a directory with a number of files in it
void Functionality::SetUp() {
    Os::Test::Directory::setUp(this->tester.get());
}

void Functionality::TearDown() {
    Os::Test::Directory::tearDown(this->tester.get());
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

// Open directory and check it is open
TEST_F(Functionality, OpenExclusive) {
    Os::Test::Directory::Tester::OpenAlreadyExistsError open_existing_rule;
    open_existing_rule.apply(*tester);
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

// Read file from directory using read(StringBase&) overload
TEST_F(Functionality, ReadOneFileString) {
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::ReadOneFileString read_str_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::IsOpen is_open_rule;
    open_rule.apply(*tester);
    is_open_rule.apply(*tester);
    read_str_rule.apply(*tester);
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
    Os::Test::Directory::Tester::GetFileCount file_count_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::IsOpen is_open_rule;
    open_rule.apply(*tester);
    is_open_rule.apply(*tester);
    file_count_rule.apply(*tester);
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

// Read a closed directory and expect an error
TEST_F(Functionality, ReadClosedDirectory) {
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::ReadWithoutOpen read_closed_rule;
    open_rule.apply(*tester);
    close_rule.apply(*tester);
    read_closed_rule.apply(*tester);
}

// Rewind a closed directory and expect an error
TEST_F(Functionality, RewindClosedDirectory) {
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::RewindWithoutOpen rewind_closed_rule;
    open_rule.apply(*tester);
    close_rule.apply(*tester);
    rewind_closed_rule.apply(*tester);
}

TEST_F(Functionality, RandomizedTesting) {
    // Enumerate all rules and construct an instance of each
    Os::Test::Directory::Tester::Open open_rule;
    Os::Test::Directory::Tester::Close close_rule;
    Os::Test::Directory::Tester::IsOpen is_open_rule;
    Os::Test::Directory::Tester::IsNotOpen is_not_open_rule;
    Os::Test::Directory::Tester::ReadOneFile read_rule;
    Os::Test::Directory::Tester::ReadOneFileString read_str_rule;
    Os::Test::Directory::Tester::Rewind rewind_rule;
    Os::Test::Directory::Tester::ReadAllFiles read_all_rule;
    Os::Test::Directory::Tester::GetFileCount file_count_rule;
    Os::Test::Directory::Tester::ReadWithoutOpen read_closed_rule;
    Os::Test::Directory::Tester::RewindWithoutOpen rewind_closed_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::Directory::Tester>* rules[] = {
            &open_rule,
            &close_rule,
            &is_open_rule,
            &is_not_open_rule,
            &read_rule,
            &read_str_rule,
            &rewind_rule,
            &read_all_rule,
            &file_count_rule,
            &read_closed_rule,
            &rewind_closed_rule
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
