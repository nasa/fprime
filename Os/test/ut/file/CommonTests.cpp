// ======================================================================
// \title Os/test/ut/file/CommonTests.cpp
// \brief common test implementations
// ======================================================================
#include "Os/test/ut/file/CommonTests.hpp"
#include <gtest/gtest.h>
#include "Os/File.hpp"

static const U32 RANDOM_BOUND = 1000;

Os::Test::FileTest::Tester::Tester() {
    // Wipe out the file system with a fresh copy
    SyntheticFile::setFileSystem(std::unique_ptr<SyntheticFileSystem>(new SyntheticFileSystem()));
}

Functionality::Functionality() : tester(Os::Test::FileTest::get_tester_implementation()) {}

void Functionality::SetUp() {
    Os::Test::FileTest::setUp(false);
}

void Functionality::TearDown() {
    Os::Test::FileTest::tearDown();
}

void FunctionalIO::SetUp() {
    // Check that the tester supports functional tests
    if (this->tester->functional()) {
        this->Functionality::SetUp();
    } else {
        GTEST_SKIP() << "Tester does not support functional i/o testing";
    }
}

// Ensure that open mode changes work reliably
TEST_F(Functionality, OpenWithCreation) {
    Os::Test::FileTest::Tester::OpenFileCreate rule(false);
    rule.apply(*tester);
}

// Ensure that close mode changes work reliably
TEST_F(Functionality, Close) {
    Os::Test::FileTest::Tester::OpenFileCreate create_rule(false);
    Os::Test::FileTest::Tester::CloseFile close_rule;
    create_rule.apply(*tester);
    close_rule.apply(*tester);
}

// Ensure that the assignment operator works correctly
TEST_F(Functionality, AssignmentOperator) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::CopyAssignment copy_rule;
    Os::Test::FileTest::Tester::CloseFile close_rule;
    open_rule.apply(*tester);
    copy_rule.apply(*tester);
    close_rule.apply(*tester);
}

// Ensure the copy constructor works correctly
TEST_F(Functionality, CopyConstructor) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::CopyConstruction copy_rule;
    Os::Test::FileTest::Tester::CloseFile close_rule;
    open_rule.apply(*tester);
    copy_rule.apply(*tester);
    close_rule.apply(*tester);
}

// Ensure that open on existence works
TEST_F(FunctionalIO, OpenWithCreationExists) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::CloseFile close_rule;
    open_rule.apply(*tester);
    close_rule.apply(*tester);
    open_rule.apply(*tester);
}

// Ensure that open on existence with overwrite works
TEST_F(Functionality, OpenWithCreationOverwrite) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::OpenFileCreateOverwrite open_overwrite(false);
    Os::Test::FileTest::Tester::CloseFile close_rule;
    open_rule.apply(*tester);
    close_rule.apply(*tester);
    open_overwrite.apply(*tester);
}

// Ensure that open mode changes work reliably
TEST_F(Functionality, OpenInvalidModes) {
    Os::Test::FileTest::Tester::OpenFileCreate original_open(false);
    Os::Test::FileTest::Tester::OpenInvalidModes invalid_open;
    original_open.apply(*tester);
    invalid_open.apply(*tester);
}

// Ensure that Os::File properly refuses preallocate calls when not open
TEST_F(Functionality, PreallocateWithoutOpen) {
    Os::Test::FileTest::Tester::PreallocateWithoutOpen rule;
    rule.apply(*tester);
}

// Ensure that Os::File properly refuses seek calls when not open
TEST_F(Functionality, SeekWithoutOpen) {
    Os::Test::FileTest::Tester::SeekWithoutOpen rule;
    rule.apply(*tester);
}

// Ensure that Os::File properly refuses seek calls when not open
TEST_F(FunctionalIO, SeekInvalidSize) {
    Os::Test::FileTest::Tester::OpenFileCreate original_open(false);
    Os::Test::FileTest::Tester::SeekInvalidSize rule;
    original_open.apply(*tester);
    rule.apply(*tester);
}

// Ensure that Os::File properly refuses flush calls when not open and when reading
TEST_F(Functionality, FlushInvalidModes) {
    Os::Test::FileTest::Tester::FlushInvalidModes flush_rule;
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::CloseFile close_rule;
    Os::Test::FileTest::Tester::OpenForRead open_read;

    // Test flush in closed state
    flush_rule.apply(*tester);

    // Used to create the test file an open in read-mode correctly
    open_rule.apply(*tester);
    close_rule.apply(*tester);
    tester->assert_file_closed();
    open_read.apply(*tester);
    tester->assert_file_opened(tester->m_current_path);

    // Check that a read-mode file cannot flush
    flush_rule.apply(*tester);
}

// Ensure that Os::File properly refuses read calls when not open and when reading
TEST_F(Functionality, ReadInvalidModes) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::CloseFile close_rule;
    Os::Test::FileTest::Tester::OpenForWrite open_write;
    Os::Test::FileTest::Tester::ReadInvalidModes read_rule;

    // Test read in closed state
    read_rule.apply(*tester);
    tester->assert_file_closed();

    // Used to create the test file and ensure reads cannot happen in read-mode
    open_rule.apply(*tester);
    read_rule.apply(*tester);
    close_rule.apply(*tester);
    tester->assert_file_closed();

    // Used to open (now existent) file in write-mode
    open_write.apply(*tester);
    tester->assert_file_opened(tester->m_current_path);

    // Check that a read won't work on write-mode data
    read_rule.apply(*tester);
}

// Ensure that Os::File properly refuses write calls when not open and when reading
TEST_F(Functionality, WriteInvalidModes) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::CloseFile close_rule;
    Os::Test::FileTest::Tester::OpenForRead open_read;
    Os::Test::FileTest::Tester::WriteInvalidModes write_rule;

    // Test write in closed state
    write_rule.apply(*tester);
    tester->assert_file_closed();

    // Used to create the test file in read-mode correctly
    open_rule.apply(*tester);
    close_rule.apply(*tester);
    tester->assert_file_closed();
    open_read.apply(*tester);
    tester->assert_file_opened(tester->m_current_path);

    // Check that a write won't work on write-mode data
    write_rule.apply(*tester);
}

// Ensure a write followed by a read produces valid data
TEST_F(FunctionalIO, WriteSeekAppendReadBack) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::OpenForAppend open_append_rule(false);
    Os::Test::FileTest::Tester::Write write_rule;
    Os::Test::FileTest::Tester::Seek seek_rule;
    Os::Test::FileTest::Tester::CloseFile close_rule;
    Os::Test::FileTest::Tester::OpenForRead open_read;
    Os::Test::FileTest::Tester::Read read_rule;

    open_rule.apply(*tester);
    write_rule.apply(*tester);
    close_rule.apply(*tester);
    open_read.apply(*tester);
    close_rule.apply(*tester);
    open_append_rule.apply(*tester);
    write_rule.apply(*tester);
    seek_rule.apply(*tester);
    write_rule.apply(*tester);
    close_rule.apply(*tester);
    open_read.apply(*tester);
    read_rule.apply(*tester);
    close_rule.apply(*tester);
}

// Ensure a write followed by a read produces valid data
TEST_F(FunctionalIO, WriteReadBack) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::Write write_rule;
    Os::Test::FileTest::Tester::CloseFile close_rule;
    Os::Test::FileTest::Tester::OpenForRead open_read;
    Os::Test::FileTest::Tester::Read read_rule;

    open_rule.apply(*tester);
    write_rule.apply(*tester);
    close_rule.apply(*tester);
    open_read.apply(*tester);
    read_rule.apply(*tester);
}

// Ensure a write followed by a read produces valid data
TEST_F(FunctionalIO, WriteReadSeek) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::Write write_rule;
    Os::Test::FileTest::Tester::CloseFile close_rule;
    Os::Test::FileTest::Tester::OpenForRead open_read;
    Os::Test::FileTest::Tester::Read read_rule;
    Os::Test::FileTest::Tester::Seek seek_rule;

    open_rule.apply(*tester);
    write_rule.apply(*tester);
    close_rule.apply(*tester);
    open_read.apply(*tester);
    read_rule.apply(*tester);
    seek_rule.apply(*tester);
}

// Ensure a write followed by a full crc produces valid results
TEST_F(FunctionalIO, WriteFullCrc) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::Write write_rule;
    Os::Test::FileTest::Tester::CloseFile close_rule;
    Os::Test::FileTest::Tester::OpenForRead open_read;
    Os::Test::FileTest::Tester::FullCrc crc_rule;

    open_rule.apply(*tester);
    write_rule.apply(*tester);
    close_rule.apply(*tester);
    open_read.apply(*tester);
    crc_rule.apply(*tester);
}

// Ensure a write followed by a partial crc produces valid results
TEST_F(FunctionalIO, WritePartialCrc) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::Write write_rule;
    Os::Test::FileTest::Tester::CloseFile close_rule;
    Os::Test::FileTest::Tester::OpenForRead open_read;
    Os::Test::FileTest::Tester::IncrementalCrc crc_rule;
    Os::Test::FileTest::Tester::FinalizeCrc finalize_rule;

    open_rule.apply(*tester);
    write_rule.apply(*tester);
    close_rule.apply(*tester);
    open_read.apply(*tester);
    crc_rule.apply(*tester);
    finalize_rule.apply(*tester);
}

// Ensure a preallocate produces valid sizes
TEST_F(FunctionalIO, Flush) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::Write write_rule;
    Os::Test::FileTest::Tester::Flush flush_rule;

    open_rule.apply(*tester);
    write_rule.apply(*tester);
    flush_rule.apply(*tester);
}

// Ensure a preallocate produces valid sizes
TEST_F(FunctionalIO, Preallocate) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::Preallocate preallocate_rule;

    open_rule.apply(*tester);
    preallocate_rule.apply(*tester);
}

// Randomized testing on the interfaces
TEST_F(Functionality, RandomizedInterfaceTesting) {
    // Enumerate all rules and construct an instance of each
    Os::Test::FileTest::Tester::OpenFileCreateOverwrite open_file_create_overwrite_rule(true);
    Os::Test::FileTest::Tester::CloseFile close_file_rule;
    Os::Test::FileTest::Tester::CopyConstruction copy_construction;
    Os::Test::FileTest::Tester::CopyAssignment copy_assignment;
    Os::Test::FileTest::Tester::OpenInvalidModes open_invalid_modes_rule;
    Os::Test::FileTest::Tester::PreallocateWithoutOpen preallocate_without_open_rule;
    Os::Test::FileTest::Tester::SeekWithoutOpen seek_without_open_rule;
    Os::Test::FileTest::Tester::FlushInvalidModes flush_invalid_modes_rule;
    Os::Test::FileTest::Tester::ReadInvalidModes read_invalid_modes_rule;
    Os::Test::FileTest::Tester::WriteInvalidModes write_invalid_modes_rule;
    Os::Test::FileTest::Tester::OpenIllegalPath open_illegal_path;
    Os::Test::FileTest::Tester::OpenIllegalMode open_illegal_mode;
    Os::Test::FileTest::Tester::SeekIllegal seek_illegal;
    Os::Test::FileTest::Tester::ReadIllegalBuffer read_illegal_buffer;
    Os::Test::FileTest::Tester::WriteIllegalBuffer write_illegal_buffer;
    Os::Test::FileTest::Tester::IncrementalCrcInvalidModes incremental_invalid_mode_rule;
    Os::Test::FileTest::Tester::FullCrcInvalidModes full_invalid_mode_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::FileTest::Tester>* rules[] = {&open_file_create_overwrite_rule,
                                                        &close_file_rule,
                                                        &copy_assignment,
                                                        &copy_construction,
                                                        &open_invalid_modes_rule,
                                                        &preallocate_without_open_rule,
                                                        &seek_without_open_rule,
                                                        &flush_invalid_modes_rule,
                                                        &read_invalid_modes_rule,
                                                        &write_invalid_modes_rule,
                                                        &open_illegal_path,
                                                        &open_illegal_mode,
                                                        &seek_illegal,
                                                        &read_illegal_buffer,
                                                        &write_illegal_buffer,
                                                        &incremental_invalid_mode_rule,
                                                        &full_invalid_mode_rule};

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::FileTest::Tester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::FileTest::Tester> bounded("Bounded Random Rules Scenario", random,
                                                               RANDOM_BOUND / 10);
    // Run!
    const U32 numSteps = bounded.run(*tester);
    printf("Ran %u steps.\n", numSteps);
}

// Ensure a write followed by a read produces valid data
TEST_F(FunctionalIO, RandomizedTesting) {
    // Enumerate all rules and construct an instance of each
    Os::Test::FileTest::Tester::OpenFileCreate open_file_create_rule(true);
    Os::Test::FileTest::Tester::OpenFileCreateOverwrite open_file_create_overwrite_rule(true);
    Os::Test::FileTest::Tester::OpenForWrite open_for_write_rule(true);
    Os::Test::FileTest::Tester::OpenForRead open_for_read_rule(true);
    Os::Test::FileTest::Tester::CloseFile close_file_rule;
    Os::Test::FileTest::Tester::Read read_rule;
    Os::Test::FileTest::Tester::Write write_rule;
    Os::Test::FileTest::Tester::Seek seek_rule;
    Os::Test::FileTest::Tester::Preallocate preallocate_rule;
    Os::Test::FileTest::Tester::Flush flush_rule;
    Os::Test::FileTest::Tester::CopyConstruction copy_construction;
    Os::Test::FileTest::Tester::CopyAssignment copy_assignment;
    Os::Test::FileTest::Tester::IncrementalCrc incremental_crc_rule;
    Os::Test::FileTest::Tester::FinalizeCrc finalize_crc_rule;
    Os::Test::FileTest::Tester::FullCrc full_crc_rule;
    Os::Test::FileTest::Tester::OpenInvalidModes open_invalid_modes_rule;
    Os::Test::FileTest::Tester::PreallocateWithoutOpen preallocate_without_open_rule;
    Os::Test::FileTest::Tester::SeekWithoutOpen seek_without_open_rule;
    Os::Test::FileTest::Tester::SeekInvalidSize seek_invalid_size;
    Os::Test::FileTest::Tester::FlushInvalidModes flush_invalid_modes_rule;
    Os::Test::FileTest::Tester::ReadInvalidModes read_invalid_modes_rule;
    Os::Test::FileTest::Tester::WriteInvalidModes write_invalid_modes_rule;
    Os::Test::FileTest::Tester::IncrementalCrcInvalidModes incremental_invalid_mode_rule;
    Os::Test::FileTest::Tester::FullCrcInvalidModes full_invalid_mode_rule;

    // Place these rules into a list of rules
    STest::Rule<Os::Test::FileTest::Tester>* rules[] = {&open_file_create_rule,
                                                        &open_file_create_overwrite_rule,
                                                        &open_for_write_rule,
                                                        &open_for_read_rule,
                                                        &close_file_rule,
                                                        &copy_assignment,
                                                        &copy_construction,
                                                        &read_rule,
                                                        &write_rule,
                                                        &seek_rule,
                                                        &preallocate_rule,
                                                        &flush_rule,
                                                        &incremental_crc_rule,
                                                        &finalize_crc_rule,
                                                        &full_crc_rule,
                                                        &open_invalid_modes_rule,
                                                        &preallocate_without_open_rule,
                                                        &seek_without_open_rule,
                                                        &seek_invalid_size,
                                                        &flush_invalid_modes_rule,
                                                        &read_invalid_modes_rule,
                                                        &write_invalid_modes_rule,
                                                        &incremental_invalid_mode_rule,
                                                        &full_invalid_mode_rule};

    // Take the rules and place them into a random scenario
    STest::RandomScenario<Os::Test::FileTest::Tester> random("Random Rules", rules, FW_NUM_ARRAY_ELEMENTS(rules));

    // Create a bounded scenario wrapping the random scenario
    STest::BoundedScenario<Os::Test::FileTest::Tester> bounded("Bounded Random Rules Scenario", random, RANDOM_BOUND);
    // Run!
    const U32 numSteps = bounded.run(*tester);
    printf("Ran %u steps.\n", numSteps);
}

// Ensure that Os::File properly refuses fullCrc when not in write mode
TEST_F(Functionality, FullCrcInvalidMode) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::FullCrcInvalidModes rule;
    open_rule.apply(*tester);
    rule.apply(*tester);
}

// Ensure that Os::File properly refuses incrementalCrc when not in write mode
TEST_F(Functionality, IncrementalCrcInvalidMode) {
    Os::Test::FileTest::Tester::OpenFileCreate open_rule(false);
    Os::Test::FileTest::Tester::IncrementalCrcInvalidModes rule;
    open_rule.apply(*tester);
    rule.apply(*tester);
}

// Ensure open prevents nullptr as path
TEST_F(InvalidArguments, OpenBadPath) {
    Os::Test::FileTest::Tester::OpenIllegalPath rule;
    rule.apply(*tester);
}

// Ensure open prevents bad modes
TEST_F(InvalidArguments, OpenBadMode) {
    Os::Test::FileTest::Tester::OpenIllegalMode rule;
    rule.apply(*tester);
}

// Ensure preallocate prevents bad length
TEST_F(InvalidArguments, SeekAbsoluteWithNegativeLength) {
    Os::Test::FileTest::Tester::SeekIllegal rule;
    rule.apply(*tester);
}

// Ensure read prevents bad buffer pointers
TEST_F(InvalidArguments, ReadInvalidBuffer) {
    Os::Test::FileTest::Tester::ReadIllegalBuffer rule;
    rule.apply(*tester);
}

// Ensure write prevents bad buffer pointers
TEST_F(InvalidArguments, WriteInvalidBuffer) {
    Os::Test::FileTest::Tester::WriteIllegalBuffer rule;
    rule.apply(*tester);
}
