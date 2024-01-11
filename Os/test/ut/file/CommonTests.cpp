#include "Os/test/ut/file/CommonTests.hpp"
#include <gtest/gtest.h>
#include "Os/File.hpp"

//! Category of tests to check for invalid argument assertions
class InvalidArguments : public Functionality {};

//! Category of tests to check for functional i/o operations
class FunctionalIO : public Functionality {
    //! Setup function delegating to UT setup function
    virtual void SetUp() override { Os::Test::File::setUp(true); }
};

// Ensure that open mode changes work reliably
TEST_F(Functionality, OpenWithCreation) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();

    Os::Test::File::Tester::OpenFileCreate rule(false);
    rule.apply(*tester);
}

// Ensure that close mode changes work reliably
TEST_F(Functionality, Close) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();

    Os::Test::File::Tester::OpenFileCreate create_rule(false);
    Os::Test::File::Tester::CloseFile close_rule;
    create_rule.apply(*tester);
    close_rule.apply(*tester);
}

// Ensure that open mode changes work reliably
TEST_F(Functionality, OpenInvalidModes) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();

    Os::Test::File::Tester::OpenFileCreate original_open(false);
    Os::Test::File::Tester::OpenInvalidModes invalid_open;
    original_open.apply(*tester);
    invalid_open.apply(*tester);
}

// Ensure that Os::File properly refuses preallocate calls when not open
TEST_F(Functionality, PreallocateWithoutOpen) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::PreallocateWithoutOpen rule;
    rule.apply(*tester);
}

// Ensure that Os::File properly refuses seek calls when not open
TEST_F(Functionality, SeekWithoutOpen) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::SeekWithoutOpen rule;
    rule.apply(*tester);
}

// Ensure that Os::File properly refuses flush calls when not open and when reading
TEST_F(Functionality, FlushInvalidModes) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::FlushInvalidModes flush_rule;
    Os::Test::File::Tester::OpenFileCreate open_rule(false);
    Os::Test::File::Tester::CloseFile close_rule;
    Os::Test::File::Tester::OpenForRead open_read;

    // Test flush in closed state
    flush_rule.apply(*tester);

    // Used to create the test file an open in read-mode correctly
    open_rule.apply(*tester);
    close_rule.apply(*tester);
    tester->assert_file_closed();
    open_read.apply(*tester);
    tester->assert_file_opened();

    // Check that a read-mode file cannot flush
    flush_rule.apply(*tester);
}

// Ensure that Os::File properly refuses read calls when not open and when reading
TEST_F(Functionality, ReadInvalidModes) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::OpenFileCreate open_rule(false);
    Os::Test::File::Tester::CloseFile close_rule;
    Os::Test::File::Tester::OpenForWrite open_write;
    Os::Test::File::Tester::ReadInvalidModes read_rule;

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
    tester->assert_file_opened();

    // Check that a read won't work on write-mode data
    read_rule.apply(*tester);
}

// Ensure that Os::File properly refuses write calls when not open and when reading
TEST_F(Functionality, WriteInvalidModes) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::OpenFileCreate open_rule(false);
    Os::Test::File::Tester::CloseFile close_rule;
    Os::Test::File::Tester::OpenForRead open_read;
    Os::Test::File::Tester::WriteInvalidModes write_rule;

    // Test write in closed state
    write_rule.apply(*tester);
    tester->assert_file_closed();

    // Used to create the test file in read-mode correctly
    open_rule.apply(*tester);
    close_rule.apply(*tester);
    tester->assert_file_closed();
    open_read.apply(*tester);
    tester->assert_file_opened();

    // Check that a write won't work on write-mode data
    write_rule.apply(*tester);
}

// Ensure a write followed by a read produces valid data
/*TEST_F(FunctionalIO, WriteReadBack) {
    U8 buffer[sizeof TEST_DATA + 10];
    Os::File file;
    Os::File::Status status = file.open(TEST_FILE_PATH, Os::File::OPEN_CREATE);
    ASSERT_EQ(Os::File::Status::OP_OK, status);
    FwSizeType size = static_cast<FwSizeType>(sizeof TEST_DATA);
    status = file.write(TEST_DATA, size);
    ASSERT_EQ(Os::File::Status::OP_OK, status);
    ASSERT_EQ(sizeof TEST_DATA, size);
    file.close();
    status = file.open(TEST_FILE_PATH, Os::File::OPEN_READ);
    ASSERT_EQ(Os::File::Status::OP_OK, status);
    status = file.read(buffer, size);
    ASSERT_EQ(Os::File::Status::OP_OK, status);
    ASSERT_EQ(sizeof TEST_DATA, size);
    ASSERT_STREQ(TEST_DATA, reinterpret_cast<CHAR*>(buffer));
}
*/

// Ensure open prevents nullptr as path
TEST_F(InvalidArguments, OpenBadPath) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::OpenIllegalPath rule;
    rule.apply(*tester);
}

// Ensure open prevents bad modes
TEST_F(InvalidArguments, OpenBadMode) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::OpenIllegalMode rule;
    rule.apply(*tester);
}

// Ensure preallocate prevents bad offset
TEST_F(InvalidArguments, PreallocateBadOffset) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::PreallocateIllegalOffset rule;
    rule.apply(*tester);
}

// Ensure preallocate prevents bad length
TEST_F(InvalidArguments, PreallocateBadLength) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::PreallocateIllegalLength rule;
    rule.apply(*tester);
}

// Ensure preallocate prevents bad length
TEST_F(InvalidArguments, SeekAbsoluteWithNegativeLength) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::SeekIllegal rule;
    rule.apply(*tester);
}

// Ensure read prevents bad buffer pointers
TEST_F(InvalidArguments, ReadInvalidBuffer) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::ReadIllegalBuffer rule;
    rule.apply(*tester);
}

// Ensure read prevents bad sizes
TEST_F(InvalidArguments, ReadInvalidSize) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::ReadIllegalSize rule;
    rule.apply(*tester);
}

// Ensure write prevents bad buffer pointers
TEST_F(InvalidArguments, WriteInvalidBuffer) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::WriteIllegalBuffer rule;
    rule.apply(*tester);
}

// Ensure write prevents bad sizes
TEST_F(InvalidArguments, WriteInvalidSize) {
    std::unique_ptr<Os::Test::File::Tester> tester = Os::Test::File::get_tester_implementation();
    Os::Test::File::Tester::WriteIllegalSize rule;
    rule.apply(*tester);
}