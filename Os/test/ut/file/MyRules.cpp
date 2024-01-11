

#include "RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"

// ------------------------------------------------------------------------------------------------------
//  OpenFile: base rule for all open rules
//
// ------------------------------------------------------------------------------------------------------
Os::Test::File::Tester::OpenBaseRule::OpenBaseRule(const char* rule_name, const bool randomize_filename)
    : STest::Rule<Os::Test::File::Tester>(rule_name), random(randomize_filename) {}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenFileCreate
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenFileCreate::OpenFileCreate(const bool overwrite, const bool randomize_filename)
    : Os::Test::File::Tester::OpenBaseRule("OpenFileCreate", randomize_filename), file_overwrite(overwrite) {}

bool Os::Test::File::Tester::OpenFileCreate::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return (state.can_create_file()) && (state.mode == Os::File::Mode::OPEN_NO_MODE);
}

void Os::Test::File::Tester::OpenFileCreate::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    std::shared_ptr<const char> filename = state.get_filename(this->random);
    bool existed = state.exists(filename.get());
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    Os::File::Status status = state.file.open(filename.get(), Os::File::Mode::OPEN_CREATE, this->file_overwrite);
    if (existed and not this->file_overwrite) {
        ASSERT_NE(Os::File::Status::OP_OK, status);
        state.assert_file_closed();
        state.mode = Os::File::Mode::OPEN_NO_MODE;
    } else {
        ASSERT_EQ(Os::File::Status::OP_OK, status) << "File creation should have been possible";
        state.assert_file_opened();
        ASSERT_EQ(state.size(filename.get()), 0);
        state.mode = Os::File::Mode::OPEN_CREATE;
    }
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForWrite
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenForWrite::OpenForWrite(const bool randomize_filename)
    : Os::Test::File::Tester::OpenBaseRule("OpenForWrite", randomize_filename) {}

bool Os::Test::File::Tester::OpenForWrite::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return (state.mode == Os::File::OPEN_NO_MODE);
}

void Os::Test::File::Tester::OpenForWrite::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    // Random selections
    std::shared_ptr<const char> filename = state.get_filename(this->random);
    bool existed = state.exists(filename.get());
    bool overwrite = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    // These asserts ensure the mode enumeration remains compact for write modes. If these fail, a better random picker
    // will be necessary.
    static_assert((Os::File::Mode::OPEN_SYNC_WRITE - 1) == Os::File::Mode::OPEN_WRITE, "Write modes not contiguous");
    static_assert((Os::File::Mode::OPEN_SYNC_DIRECT_WRITE - 1) == Os::File::Mode::OPEN_SYNC_WRITE,
                  "Write modes not contiguous");
    static_assert((Os::File::Mode::OPEN_APPEND - 1) == Os::File::Mode::OPEN_SYNC_DIRECT_WRITE,
                  "Write modes not contiguous");

    Os::File::Mode random_write_mode =
        static_cast<Os::File::Mode>(STest::Pick::lowerUpper(Os::File::Mode::OPEN_WRITE, Os::File::Mode::OPEN_APPEND));
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    Os::File::Status status = state.file.open(filename.get(), random_write_mode, overwrite);
    if (existed) {
        ASSERT_EQ(Os::File::Status::OP_OK, status);
        state.assert_file_opened();
        state.mode = random_write_mode;
        ASSERT_EQ(random_write_mode, state.file.mode);
    } else {
        ASSERT_NE(Os::File::Status::OP_OK, status);
        state.assert_file_closed();
        state.mode = Os::File::Mode::OPEN_NO_MODE;
        ASSERT_EQ(Os::File::Mode::OPEN_NO_MODE, state.file.mode);
    }
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForRead
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenForRead::OpenForRead(const bool randomize_filename)
    : Os::Test::File::Tester::OpenBaseRule("OpenForRead", randomize_filename) {}

bool Os::Test::File::Tester::OpenForRead::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return (state.mode == Os::File::OPEN_NO_MODE);
}

void Os::Test::File::Tester::OpenForRead::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    // Random selections
    std::shared_ptr<const char> filename = state.get_filename(this->random);
    bool existed = state.exists(filename.get());
    bool overwrite = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    Os::File::Status status = state.file.open(filename.get(), Os::File::OPEN_READ, overwrite);
    if (existed) {
        ASSERT_EQ(Os::File::Status::OP_OK, status);
        state.assert_file_opened();
        state.mode = Os::File::OPEN_READ;
        ASSERT_EQ(Os::File::OPEN_READ, state.file.mode);
    } else {
        ASSERT_NE(Os::File::Status::OP_OK, status);
        state.assert_file_closed();
        state.mode = Os::File::Mode::OPEN_NO_MODE;
        ASSERT_EQ(Os::File::Mode::OPEN_NO_MODE, state.file.mode);
    }
}

// ------------------------------------------------------------------------------------------------------
// Rule:  CloseFile
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::CloseFile::CloseFile() : STest::Rule<Os::Test::File::Tester>("CloseFile") {}

bool Os::Test::File::Tester::CloseFile::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE != state.mode;
}

void Os::Test::File::Tester::CloseFile::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    state.assert_file_opened();
    state.file.close();
    state.mode = Os::File::Mode::OPEN_NO_MODE;
    state.assert_file_closed();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenInvalidModes::OpenInvalidModes()
    : STest::Rule<Os::Test::File::Tester>("OpenInvalidModes") {}

bool Os::Test::File::Tester::OpenInvalidModes::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE != state.mode;
}

void Os::Test::File::Tester::OpenInvalidModes::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    // Check initial file state
    state.assert_file_opened();
    std::shared_ptr<const char> filename = state.get_filename(true);
    Os::File::Status status = state.file.open(filename.get(), Os::File::Mode::OPEN_CREATE);
    state.assert_valid_mode_status(status);
    state.assert_file_opened();  // File remains open
}

// ------------------------------------------------------------------------------------------------------
// Rule:  PreallocateWithoutOpen
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::PreallocateWithoutOpen::PreallocateWithoutOpen()
    : STest::Rule<Os::Test::File::Tester>("PreallocateWithoutOpen") {}

bool Os::Test::File::Tester::PreallocateWithoutOpen::precondition(
    const Os::Test::File::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.mode;
}

void Os::Test::File::Tester::PreallocateWithoutOpen::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    FwSizeType random_offset = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());
    FwSizeType random_size = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());

    Os::File::Status status = state.file.preallocate(random_offset, random_size);
    state.assert_valid_mode_status(status);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekWithoutOpen
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::SeekWithoutOpen::SeekWithoutOpen() : STest::Rule<Os::Test::File::Tester>("SeekWithoutOpen") {}

bool Os::Test::File::Tester::SeekWithoutOpen::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.mode;
}

void Os::Test::File::Tester::SeekWithoutOpen::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    FwSizeType random_offset = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());
    bool random_absolute = static_cast<bool>(STest::Pick::lowerUpper(0, 1));

    Os::File::Status status = state.file.seek(random_offset, random_absolute);
    state.assert_valid_mode_status(status);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  FlushInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::FlushInvalidModes::FlushInvalidModes()
    : STest::Rule<Os::Test::File::Tester>("FlushInvalidModes") {}

bool Os::Test::File::Tester::FlushInvalidModes::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.mode || Os::File::Mode::OPEN_READ == state.mode;
}

void Os::Test::File::Tester::FlushInvalidModes::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    ASSERT_TRUE(Os::File::Mode::OPEN_NO_MODE == state.file.mode || Os::File::Mode::OPEN_READ == state.file.mode);
    Os::File::Status status = state.file.flush();
    state.assert_valid_mode_status(status);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::ReadInvalidModes::ReadInvalidModes()
    : STest::Rule<Os::Test::File::Tester>("ReadInvalidModes") {}

bool Os::Test::File::Tester::ReadInvalidModes::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_READ != state.mode;
}

void Os::Test::File::Tester::ReadInvalidModes::action(Os::Test::File::Tester& state  //!< The test state
) {
    U8 buffer[10];
    FwSizeType size = sizeof buffer;
    printf("--> Rule: %s \n", this->name);
    ASSERT_NE(Os::File::Mode::OPEN_READ, state.file.mode);
    Os::File::Status status = state.file.read(buffer, size, static_cast<bool>(STest::Pick::lowerUpper(0, 1)));
    state.assert_valid_mode_status(status);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  WriteInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::WriteInvalidModes::WriteInvalidModes()
    : STest::Rule<Os::Test::File::Tester>("WriteInvalidModes") {}

bool Os::Test::File::Tester::WriteInvalidModes::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.mode || Os::File::Mode::OPEN_READ == state.mode;
}

void Os::Test::File::Tester::WriteInvalidModes::action(Os::Test::File::Tester& state  //!< The test state
) {
    U8 buffer[10];
    FwSizeType size = sizeof buffer;
    printf("--> Rule: %s \n", this->name);
    ASSERT_TRUE(Os::File::Mode::OPEN_NO_MODE == state.file.mode || Os::File::Mode::OPEN_READ == state.file.mode);
    Os::File::Status status = state.file.write(buffer, size, static_cast<bool>(STest::Pick::lowerUpper(0, 1)));
    state.assert_valid_mode_status(status);
}

// ------------------------------------------------------------------------------------------------------
// Base Rule:  AssertRule
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::AssertRule::AssertRule(const char* name) : STest::Rule<Os::Test::File::Tester>(name) {}

bool Os::Test::File::Tester::AssertRule::precondition(const Os::Test::File::Tester& state  //!< The test state
) {
    return true;
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenIllegalPath
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenIllegalPath::OpenIllegalPath() : Os::Test::File::Tester::AssertRule("OpenIllegalPath") {}

void Os::Test::File::Tester::OpenIllegalPath::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    Os::File::Mode random_mode =
        static_cast<Os::File::Mode>(STest::Pick::lowerUpper(Os::File::Mode::OPEN_READ, Os::File::Mode::OPEN_APPEND));
    bool overwrite = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.open(nullptr, random_mode, overwrite), ASSERT_IN_FILE_CPP);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenIllegalMode
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenIllegalMode::OpenIllegalMode() : Os::Test::File::Tester::AssertRule("OpenIllegalMode") {}

void Os::Test::File::Tester::OpenIllegalMode::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    std::shared_ptr<const char> random_filename = state.get_filename(true);
    bool overwrite = static_cast<bool>(STest::Pick::lowerUpper(0, 1));

    // Loop until we find a non-valid mode randomly
    U32 any = 0;
    do {
        any = STest::Pick::any();
    } while ((any != static_cast<U32>(Os::File::Mode::OPEN_NO_MODE)) &&
             (any < static_cast<U32>(Os::File::Mode::MAX_OPEN_MODE)));
    ASSERT_DEATH_IF_SUPPORTED(state.file.open(random_filename.get(), static_cast<Os::File::Mode>(any), overwrite),
                              Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  PreallocateIllegalOffset
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::PreallocateIllegalOffset::PreallocateIllegalOffset()
    : Os::Test::File::Tester::AssertRule("PreallocateIllegalOffset") {}

void Os::Test::File::Tester::PreallocateIllegalOffset::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    FwSizeType length = static_cast<FwSizeType>(STest::Pick::any());
    FwSizeType invalid_offset = -1 * static_cast<FwSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    ASSERT_DEATH_IF_SUPPORTED(state.file.preallocate(invalid_offset, length),
                              Os::Test::File::Tester::ASSERT_IN_FILE_CPP) << "With offset: " << invalid_offset;
}

// ------------------------------------------------------------------------------------------------------
// Rule:  PreallocateIllegalLength
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::PreallocateIllegalLength::PreallocateIllegalLength()
    : Os::Test::File::Tester::AssertRule("PreallocateIllegalLength") {}

void Os::Test::File::Tester::PreallocateIllegalLength::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    FwSizeType offset = static_cast<FwSizeType>(STest::Pick::any());
    FwSizeType invalid_length = -1 * static_cast<FwSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    ASSERT_DEATH_IF_SUPPORTED(state.file.preallocate(offset, invalid_length),
                              Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekIllegal
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::SeekIllegal::SeekIllegal() : Os::Test::File::Tester::AssertRule("SeekIllegal") {}

void Os::Test::File::Tester::SeekIllegal::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    ASSERT_DEATH_IF_SUPPORTED(state.file.seek(-1, true), Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadIllegalBuffer
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::ReadIllegalBuffer::ReadIllegalBuffer()
    : Os::Test::File::Tester::AssertRule("ReadIllegalBuffer") {}

void Os::Test::File::Tester::ReadIllegalBuffer::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    FwSizeType size = static_cast<FwSizeType>(STest::Pick::any());
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.read(nullptr, size, random_wait), Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadIllegalSize
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::ReadIllegalSize::ReadIllegalSize() : Os::Test::File::Tester::AssertRule("ReadIllegalSize") {}

void Os::Test::File::Tester::ReadIllegalSize::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    U8 buffer[10] = {};
    FwSizeType invalid_size = -1 * static_cast<FwSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.read(buffer, invalid_size, random_wait),
                              Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  WriteIllegalBuffer
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::WriteIllegalBuffer::WriteIllegalBuffer()
    : Os::Test::File::Tester::AssertRule("WriteIllegalBuffer") {}

void Os::Test::File::Tester::WriteIllegalBuffer::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    FwSizeType size = static_cast<FwSizeType>(STest::Pick::any());
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.write(nullptr, size, random_wait), Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  WriteIllegalSize
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::WriteIllegalSize::WriteIllegalSize() : Os::Test::File::Tester::AssertRule("WriteIllegalSize") {}

void Os::Test::File::Tester::WriteIllegalSize::action(Os::Test::File::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    U8 buffer[10] = {};
    FwSizeType invalid_size = -1 * static_cast<FwSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.read(buffer, invalid_size, random_wait),
                              Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
}
