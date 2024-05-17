// ======================================================================
// \title Os/test/ut/file/MyRules.cpp
// \brief rule implementations for common testing
// ======================================================================

#include "RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"
extern "C" {
#include <Utils/Hash/libcrc/lib_crc.h> // borrow CRC
}

// For testing, limit files to 32K
const FwSignedSizeType FILE_DATA_MAXIMUM = 32 * 1024;

Os::File::Status Os::Test::File::Tester::shadow_open(const std::string &path, Os::File::Mode open_mode, bool overwrite) {
    Os::File::Status status = this->m_shadow.open(path.c_str(), open_mode, overwrite ? Os::File::OverwriteType::OVERWRITE : Os::File::OverwriteType::NO_OVERWRITE);
    if (Os::File::Status::OP_OK == status) {
        this->m_current_path = path;
        this->m_mode = open_mode;
        this->m_independent_crc = Os::File::INITIAL_CRC;
    } else {
        this->m_current_path.clear();
        this->m_mode = Os::File::Mode::OPEN_NO_MODE;
    }
    return status;
}

void Os::Test::File::Tester::shadow_close() {
    this->m_shadow.close();
    this->m_current_path.clear();
    this->m_mode = Os::File::Mode::OPEN_NO_MODE;
    // Checks on the shadow data to ensure consistency
    ASSERT_TRUE(this->m_current_path.empty());
}

std::vector<U8> Os::Test::File::Tester::shadow_read(FwSignedSizeType size) {
    std::vector<U8> output;
    output.resize(size);
    Os::File::Status status = m_shadow.read(output.data(), size, Os::File::WaitType::WAIT);
    output.resize(size);
    EXPECT_EQ(status, Os::File::Status::OP_OK);
    return output;
}

void Os::Test::File::Tester::shadow_write(const std::vector<U8>& write_data) {
    FwSignedSizeType size = static_cast<FwSignedSizeType>(write_data.size());
    FwSignedSizeType original_size = size;
    Os::File::Status status = Os::File::OP_OK;
    if (write_data.data() != nullptr) {
        status = m_shadow.write(write_data.data(), size, Os::File::WaitType::WAIT);
    }
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    ASSERT_EQ(size, original_size);
}

void Os::Test::File::Tester::shadow_seek(const FwSignedSizeType offset, const bool absolute) {
    Os::File::Status status = m_shadow.seek(offset, absolute ?Os::File::SeekType::ABSOLUTE : Os::File::SeekType::RELATIVE);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
}

void Os::Test::File::Tester::shadow_preallocate(const FwSignedSizeType offset, const FwSignedSizeType length) {
    Os::File::Status status = m_shadow.preallocate(offset, length);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
}

void Os::Test::File::Tester::shadow_flush() {
    Os::File::Status status = m_shadow.flush();
    ASSERT_EQ(status, Os::File::Status::OP_OK);
}

void Os::Test::File::Tester::shadow_crc(U32& crc) {
    crc = this->m_independent_crc;
    SyntheticFileData& data = *reinterpret_cast<SyntheticFileData*>(this->m_shadow.getHandle());

    // Calculate CRC on full file starting at m_pointer
    for (FwSizeType i = data.m_pointer; i < data.m_data.size(); i++, this->m_shadow.seek(1, Os::File::SeekType::RELATIVE)) {
        crc = update_crc_32(crc, static_cast<char>(data.m_data.at(i)));
    }
    // Update tracking variables
    this->m_independent_crc = Os::File::INITIAL_CRC;
}

void Os::Test::File::Tester::shadow_partial_crc(FwSignedSizeType& size) {
    SyntheticFileData data = *reinterpret_cast<SyntheticFileData*>(this->m_shadow.getHandle());

    // Calculate CRC on full file starting at m_pointer
    const FwSizeType bound = FW_MIN(static_cast<FwSizeType>(data.m_pointer) + size, data.m_data.size());
    size = FW_MAX(0, static_cast<FwSignedSizeType>(bound - data.m_pointer));
    for (FwSizeType i = data.m_pointer; i < bound; i++) {
        this->m_independent_crc = update_crc_32(this->m_independent_crc, static_cast<char>(data.m_data.at(i)));
        this->m_shadow.seek(1, Os::File::SeekType::RELATIVE);
    }
}

void Os::Test::File::Tester::shadow_finalize(U32& crc) {
    crc = this->m_independent_crc;
    this->m_independent_crc = Os::File::INITIAL_CRC;
}


Os::Test::File::Tester::FileState Os::Test::File::Tester::current_file_state() {
    Os::Test::File::Tester::FileState state;
    // Invariant: mode must not be closed, or path must be nullptr
    EXPECT_TRUE((Os::File::Mode::OPEN_NO_MODE != this->m_file.m_mode) || (nullptr == this->m_file.m_path));

    // Read state when file is open
    if (Os::File::Mode::OPEN_NO_MODE != this->m_file.m_mode) {
        EXPECT_EQ(this->m_file.position(state.position), Os::File::Status::OP_OK);
        EXPECT_EQ(this->m_file.size(state.size), Os::File::Status::OP_OK);
        // Extra check to ensure size does not alter pointer
        FwSignedSizeType new_position = -1;
        EXPECT_EQ(this->m_file.position(new_position), Os::File::Status::OP_OK);
        EXPECT_EQ(new_position, state.position);
    }
    return state;
}

void Os::Test::File::Tester::assert_valid_mode_status(Os::File::Status &status) const {
    if (Os::File::Mode::OPEN_NO_MODE == this->m_mode) {
        ASSERT_EQ(status, Os::File::Status::NOT_OPENED);
    } else {
        ASSERT_EQ(status, Os::File::Status::INVALID_MODE);
    }
}


void Os::Test::File::Tester::assert_file_consistent() {
    // Ensure file mode
    ASSERT_EQ(this->m_mode, this->m_file.m_mode);
    if (this->m_file.m_path == nullptr) {
        ASSERT_EQ(this->m_current_path, std::string(""));
    } else {
        // Ensure the state path matches the file path
        std::string path = std::string(this->m_file.m_path);
        ASSERT_EQ(path, this->m_current_path);

        // Check real file properties when able to do so
        if (this->functional()) {
            //  File exists, check all properties
            if (SyntheticFile::exists(this->m_current_path.c_str())) {
                // Ensure the file pointer is consistent
                FwSignedSizeType current_position = 0;
                FwSignedSizeType shadow_position = 0;
                ASSERT_EQ(this->m_file.position(current_position), Os::File::Status::OP_OK);
                ASSERT_EQ(this->m_shadow.position(shadow_position), Os::File::Status::OP_OK);

                ASSERT_EQ(current_position, shadow_position);
                // Ensure the file size is consistent
                FwSignedSizeType current_size = 0;
                FwSignedSizeType shadow_size = 0;
                ASSERT_EQ(this->m_file.size(current_size), Os::File::Status::OP_OK);
                ASSERT_EQ(this->m_shadow.size(shadow_size), Os::File::Status::OP_OK);
                ASSERT_EQ(current_size, shadow_size);
            }
            // Does not exist
            else {
                ASSERT_FALSE(this->exists(this->m_current_path));
            }
        }
    }
}

void Os::Test::File::Tester::assert_file_opened(const std::string &path, Os::File::Mode newly_opened_mode, bool overwrite) {
    // Assert the that the file is opened in some mode
    ASSERT_NE(this->m_file.m_mode, Os::File::Mode::OPEN_NO_MODE);
    ASSERT_TRUE(this->m_file.isOpen()) << "`isOpen()` failed to indicate file is open";
    ASSERT_EQ(this->m_file.m_mode, this->m_mode);

    // When the open mode has been specified assert that is in an exact state
    if (not path.empty() && Os::File::Mode::OPEN_NO_MODE != newly_opened_mode) {
        // Assert file pointer always at beginning when functional
        if (functional() ) {
            FwSignedSizeType file_position = -1;
            ASSERT_EQ(this->m_file.position(file_position), Os::File::Status::OP_OK);
            ASSERT_EQ(file_position, 0);
        }
        ASSERT_EQ(std::string(this->m_file.m_path), path);
        ASSERT_EQ(this->m_file.m_mode, newly_opened_mode) << "File is in unexpected mode";

        // Check truncations
        const bool truncate = (Os::File::Mode::OPEN_CREATE == newly_opened_mode) && overwrite;
        if (truncate) {
            if (this->functional()) {
                FwSignedSizeType file_size = -1;
                ASSERT_EQ(this->m_file.size(file_size), Os::File::Status::OP_OK);
                ASSERT_EQ(file_size, 0);
            }
        }
    }
}

void Os::Test::File::Tester::assert_file_closed() {
    ASSERT_EQ(this->m_file.m_mode, Os::File::Mode::OPEN_NO_MODE) << "File is in unexpected mode";
    ASSERT_FALSE(this->m_file.isOpen()) << "`isOpen()` failed to indicate file is open";
}

void Os::Test::File::Tester::assert_file_read(const std::vector<U8>& state_data, const unsigned char *read_data, FwSignedSizeType size_read) {
    // Functional tests
    if (functional()) {
        ASSERT_EQ(size_read, state_data.size());
        ASSERT_EQ(std::vector<U8>(read_data, read_data + size_read), state_data);
        FwSignedSizeType position = -1;
        FwSignedSizeType shadow_position = -1;
        ASSERT_EQ(this->m_file.position(position), Os::File::Status::OP_OK);
        ASSERT_EQ(this->m_shadow.position(shadow_position), Os::File::Status::OP_OK);
        ASSERT_EQ(position, shadow_position);
    }
}

void Os::Test::File::Tester::assert_file_write(const std::vector<U8>& write_data, FwSignedSizeType size_written) {
    ASSERT_EQ(size_written, write_data.size());
    FwSignedSizeType file_size = 0;
    FwSignedSizeType shadow_size = 0;
    ASSERT_EQ(this->m_file.size(file_size), Os::File::Status::OP_OK);
    ASSERT_EQ(this->m_shadow.size(shadow_size), Os::File::Status::OP_OK);
    ASSERT_EQ(file_size, shadow_size);
    FwSignedSizeType file_position = -1;
    FwSignedSizeType shadow_position = -1;
    ASSERT_EQ(this->m_file.position(file_position), Os::File::Status::OP_OK);
    ASSERT_EQ(this->m_shadow.position(shadow_position), Os::File::Status::OP_OK);
    ASSERT_EQ(file_position, shadow_position);
}

void Os::Test::File::Tester::assert_file_seek(const FwSignedSizeType original_position, const FwSignedSizeType seek_desired, const bool absolute) {
    FwSignedSizeType new_position = 0;
    FwSignedSizeType shadow_position = 0;

    ASSERT_EQ(this->m_file.position(new_position), Os::File::Status::OP_OK);
    ASSERT_EQ(this->m_shadow.position(shadow_position), Os::File::Status::OP_OK);

    const FwSignedSizeType expected_offset = (absolute) ? seek_desired : (original_position + seek_desired);
    if (expected_offset > 0) {
        ASSERT_EQ(new_position, expected_offset);
    } else {
        ASSERT_EQ(new_position, original_position);
    }
    ASSERT_EQ(new_position, shadow_position);
}

// ------------------------------------------------------------------------------------------------------
//  OpenFile: base rule for all open rules
//
// ------------------------------------------------------------------------------------------------------
Os::Test::File::Tester::OpenBaseRule::OpenBaseRule(const char *rule_name,
                                                   Os::File::Mode mode,
                                                   const bool overwrite,
                                                   const bool randomize_filename)
        : STest::Rule<Os::Test::File::Tester>(rule_name), m_mode(mode),
          m_overwrite(overwrite ? Os::File::OverwriteType::OVERWRITE : Os::File::OverwriteType::NO_OVERWRITE),
          m_random(randomize_filename) {}

bool Os::Test::File::Tester::OpenBaseRule::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return state.m_mode == Os::File::Mode::OPEN_NO_MODE;
}

void Os::Test::File::Tester::OpenBaseRule::action(Os::Test::File::Tester &state  //!< The test state
) {
    // Initial variables used for this test
    std::shared_ptr<const std::string> filename = state.get_filename(this->m_random);

    // Ensure initial and shadow states synchronized
    state.assert_file_consistent();
    state.assert_file_closed();

    // Perform action and shadow action asserting the results are the same
    Os::File::Status status = state.m_file.open(filename->c_str(), m_mode, this->m_overwrite);
    Os::File::Status s2 = state.shadow_open(*filename, m_mode, this->m_overwrite);
    ASSERT_EQ(status, s2);

    // Extra check to ensure file is consistently open
    if (Os::File::Status::OP_OK == status) {
        state.assert_file_opened(*filename, m_mode);
        FileState file_state = state.current_file_state();
        ASSERT_EQ(file_state.position, 0); // Open always zeros the position
    }
    // Assert the file state remains consistent.
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenFileCreate
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenFileCreate::OpenFileCreate(const bool randomize_filename)
        : Os::Test::File::Tester::OpenBaseRule("OpenFileCreate", Os::File::Mode::OPEN_CREATE, false,
                                               randomize_filename) {}


// ------------------------------------------------------------------------------------------------------
// Rule:  OpenFileCreateOverwrite
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenFileCreateOverwrite::OpenFileCreateOverwrite(const bool randomize_filename)
        : Os::Test::File::Tester::OpenBaseRule("OpenFileCreate", Os::File::Mode::OPEN_CREATE, true,
                                               randomize_filename) {}


// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForWrite
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenForWrite::OpenForWrite(const bool randomize_filename)
        : Os::Test::File::Tester::OpenBaseRule("OpenForWrite",
        // Randomized write mode
                                               static_cast<Os::File::Mode>(STest::Pick::lowerUpper(
                                                       Os::File::Mode::OPEN_WRITE,
                                                       Os::File::Mode::OPEN_APPEND)),
        // Randomized overwrite
                                               static_cast<bool>(STest::Pick::lowerUpper(0, 1)),
                                               randomize_filename) {
    // Ensures that a random write mode will work correctly
    static_assert((Os::File::Mode::OPEN_SYNC_WRITE - 1) == Os::File::Mode::OPEN_WRITE, "Write modes not contiguous");
    static_assert((Os::File::Mode::OPEN_APPEND - 1) == Os::File::Mode::OPEN_SYNC_WRITE,
                  "Write modes not contiguous");

}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForRead
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenForRead::OpenForRead(const bool randomize_filename)
        : Os::Test::File::Tester::OpenBaseRule("OpenForRead", Os::File::Mode::OPEN_READ,
        // Randomized overwrite
                                               static_cast<bool>(STest::Pick::lowerUpper(0, 1)),
                                               randomize_filename) {}

// ------------------------------------------------------------------------------------------------------
// Rule:  CloseFile
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::CloseFile::CloseFile() : STest::Rule<Os::Test::File::Tester>("CloseFile") {}

bool Os::Test::File::Tester::CloseFile::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE != state.m_mode;
}

void Os::Test::File::Tester::CloseFile::action(Os::Test::File::Tester &state  //!< The test state
) {
    // Make sure test state and file state synchronized
    state.assert_file_consistent();
    state.assert_file_opened(state.m_current_path);
    // Close file and shadow state
    state.m_file.close();
    state.shadow_close();
    // Assert test state and file state synchronized
    state.assert_file_closed();
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Read
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::Read::Read() :
        STest::Rule<Os::Test::File::Tester>("Read") {
}


bool Os::Test::File::Tester::Read::precondition(
        const Os::Test::File::Tester &state //!< The test state
) {
    return Os::File::Mode::OPEN_READ == state.m_mode;
}


void Os::Test::File::Tester::Read::action(
        Os::Test::File::Tester &state //!< The test state
) {
    U8 buffer[FILE_DATA_MAXIMUM];
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    FwSignedSizeType size_desired = static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM));
    FwSignedSizeType size_read = size_desired;
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    Os::File::Status status = state.m_file.read(buffer, size_read, wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT);
    ASSERT_EQ(Os::File::Status::OP_OK, status);
    std::vector<U8> read_data = state.shadow_read(size_desired);
    state.assert_file_read(read_data, buffer, size_read);
    FileState final_file_state = state.current_file_state();
    // File size should not change during read
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Write
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::Write::Write() :
        STest::Rule<Os::Test::File::Tester>("Write") {
}


bool Os::Test::File::Tester::Write::precondition(
        const Os::Test::File::Tester &state //!< The test state
) {
    return Os::File::Mode::OPEN_CREATE <= state.m_mode;
}


void Os::Test::File::Tester::Write::action(
        Os::Test::File::Tester &state //!< The test state
) {
    U8 buffer[FILE_DATA_MAXIMUM];
    state.assert_file_consistent();
    FwSignedSizeType size_desired = static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM));
    FwSignedSizeType size_written = size_desired;
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    for (FwSignedSizeType i = 0; i < size_desired; i++) {
        buffer[i] = static_cast<U8>(STest::Pick::lowerUpper(0, std::numeric_limits<U8>::max()));
    }
    std::vector<U8> write_data(buffer, buffer + size_desired);
    Os::File::Status status = state.m_file.write(buffer, size_written, wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT);
    ASSERT_EQ(Os::File::Status::OP_OK, status);
    ASSERT_EQ(size_written, size_desired);
    state.shadow_write(write_data);
    state.assert_file_write(write_data, size_written);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Read
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::Seek::Seek() :
        STest::Rule<Os::Test::File::Tester>("Seek") {
}


bool Os::Test::File::Tester::Seek::precondition(
        const Os::Test::File::Tester &state //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE < state.m_mode;
}


void Os::Test::File::Tester::Seek::action(
        Os::Test::File::Tester &state //!< The test state
) {
    FwSignedSizeType seek_offset = 0;
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();

    // Choose some random values
    bool absolute = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    if (absolute) {
        seek_offset = STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM);
    } else {
        seek_offset = STest::Pick::lowerUpper(0, original_file_state.position + FILE_DATA_MAXIMUM) - original_file_state.position;
    }
    Os::File::Status status = state.m_file.seek(seek_offset, absolute ? Os::File::SeekType::ABSOLUTE : Os::File::SeekType::RELATIVE);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    state.shadow_seek(seek_offset, absolute);
    state.assert_file_seek(original_file_state.position, seek_offset, absolute);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Preallocate
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::Preallocate::Preallocate() :
        STest::Rule<Os::Test::File::Tester>("Preallocate") {
}


bool Os::Test::File::Tester::Preallocate::precondition(
        const Os::Test::File::Tester &state //!< The test state
) {
    return Os::File::Mode::OPEN_CREATE <= state.m_mode;
}


void Os::Test::File::Tester::Preallocate::action(
        Os::Test::File::Tester &state //!< The test state
) {
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    FwSignedSizeType offset = static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM));
    FwSignedSizeType length = static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM));
    Os::File::Status status = state.m_file.preallocate(offset, length);
    ASSERT_EQ(Os::File::Status::OP_OK, status);
    state.shadow_preallocate(offset, length);
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, FW_MAX(original_file_state.size, offset + length));
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Flush
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::Flush::Flush() :
        STest::Rule<Os::Test::File::Tester>("Flush") {
}


bool Os::Test::File::Tester::Flush::precondition(
        const Os::Test::File::Tester &state //!< The test state
) {
    return Os::File::Mode::OPEN_CREATE <= state.m_mode;
}


void Os::Test::File::Tester::Flush::action(
        Os::Test::File::Tester &state //!< The test state
) {
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    Os::File::Status status = state.m_file.flush();
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    state.shadow_flush();

    // Ensure no change in size or pointer
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}



// ------------------------------------------------------------------------------------------------------
// Rule:  OpenInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenInvalidModes::OpenInvalidModes()
        : STest::Rule<Os::Test::File::Tester>("OpenInvalidModes") {}

bool Os::Test::File::Tester::OpenInvalidModes::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE != state.m_mode;
}

void Os::Test::File::Tester::OpenInvalidModes::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    // Check initial file state
    state.assert_file_opened(state.m_current_path);
    std::shared_ptr<const std::string> filename = state.get_filename(true);
    Os::File::Status status = state.m_file.open(filename->c_str(), Os::File::Mode::OPEN_CREATE);
    state.assert_valid_mode_status(status);
    state.assert_file_opened(state.m_current_path);  // Original file remains open
    // Ensure no change in size or pointer of original file
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  PreallocateWithoutOpen
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::PreallocateWithoutOpen::PreallocateWithoutOpen()
        : STest::Rule<Os::Test::File::Tester>("PreallocateWithoutOpen") {}

bool Os::Test::File::Tester::PreallocateWithoutOpen::precondition(
        const Os::Test::File::Tester &state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.m_mode;
}

void Os::Test::File::Tester::PreallocateWithoutOpen::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    FwSignedSizeType random_offset = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());
    FwSignedSizeType random_size = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());

    Os::File::Status status = state.m_file.preallocate(random_offset, random_size);
    state.assert_valid_mode_status(status);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekWithoutOpen
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::SeekWithoutOpen::SeekWithoutOpen() : STest::Rule<Os::Test::File::Tester>("SeekWithoutOpen") {}

bool Os::Test::File::Tester::SeekWithoutOpen::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.m_mode;
}

void Os::Test::File::Tester::SeekWithoutOpen::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    FwSignedSizeType random_offset = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());
    bool random_absolute = static_cast<bool>(STest::Pick::lowerUpper(0, 1));

    Os::File::Status status = state.m_file.seek(random_offset, random_absolute ? Os::File::SeekType::ABSOLUTE : Os::File::SeekType::RELATIVE);
    state.assert_valid_mode_status(status);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekInvalidSize
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::SeekInvalidSize::SeekInvalidSize() : STest::Rule<Os::Test::File::Tester>("SeekInvalidSize") {}

bool Os::Test::File::Tester::SeekInvalidSize::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE < state.m_mode;;
}

void Os::Test::File::Tester::SeekInvalidSize::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    // Open file of given filename
    FwSignedSizeType random_offset = STest::Pick::lowerUpper(original_file_state.position + 1, std::numeric_limits<U32>::max());
    ASSERT_GT(random_offset, original_file_state.position);

    Os::File::Status status = state.m_file.seek(-1 * random_offset, Os::File::SeekType::RELATIVE);
    ASSERT_EQ(Os::File::Status::INVALID_ARGUMENT, status);
    // Ensure no change in size or pointer
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  FlushInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::FlushInvalidModes::FlushInvalidModes()
        : STest::Rule<Os::Test::File::Tester>("FlushInvalidModes") {}

bool Os::Test::File::Tester::FlushInvalidModes::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.m_mode || Os::File::Mode::OPEN_READ == state.m_mode;
}

void Os::Test::File::Tester::FlushInvalidModes::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_TRUE(Os::File::Mode::OPEN_NO_MODE == state.m_file.m_mode || Os::File::Mode::OPEN_READ == state.m_file.m_mode);
    Os::File::Status status = state.m_file.flush();
    state.assert_valid_mode_status(status);
    // Ensure no change in size or pointer
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::ReadInvalidModes::ReadInvalidModes()
        : STest::Rule<Os::Test::File::Tester>("ReadInvalidModes") {}

bool Os::Test::File::Tester::ReadInvalidModes::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return Os::File::Mode::OPEN_READ != state.m_mode;
}

void Os::Test::File::Tester::ReadInvalidModes::action(Os::Test::File::Tester &state  //!< The test state
) {
    U8 buffer[10];
    FwSignedSizeType size = sizeof buffer;
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_NE(Os::File::Mode::OPEN_READ, state.m_file.m_mode);
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    Os::File::Status status = state.m_file.read(buffer, size, wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT);
    state.assert_valid_mode_status(status);

    // Ensure no change in size or pointer
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  WriteInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::WriteInvalidModes::WriteInvalidModes()
        : STest::Rule<Os::Test::File::Tester>("WriteInvalidModes") {}

bool Os::Test::File::Tester::WriteInvalidModes::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.m_mode || Os::File::Mode::OPEN_READ == state.m_mode;
}

void Os::Test::File::Tester::WriteInvalidModes::action(Os::Test::File::Tester &state  //!< The test state
) {
    U8 buffer[10];
    FwSignedSizeType size = sizeof buffer;
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_TRUE(Os::File::Mode::OPEN_NO_MODE == state.m_file.m_mode || Os::File::Mode::OPEN_READ == state.m_file.m_mode);
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    Os::File::Status status = state.m_file.write(buffer, size, wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT);
    state.assert_valid_mode_status(status);
    // Ensure no change in size or pointer
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Base Rule:  AssertRule
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::AssertRule::AssertRule(const char *name) : STest::Rule<Os::Test::File::Tester>(name) {}

bool Os::Test::File::Tester::AssertRule::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return true;
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenIllegalPath
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenIllegalPath::OpenIllegalPath() : Os::Test::File::Tester::AssertRule("OpenIllegalPath") {}

void Os::Test::File::Tester::OpenIllegalPath::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    Os::File::Mode random_mode =
            static_cast<Os::File::Mode>(STest::Pick::lowerUpper(Os::File::Mode::OPEN_READ,
                                                                Os::File::Mode::OPEN_APPEND));
    bool overwrite = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(
            state.m_file.open(nullptr, random_mode,
                              overwrite ? Os::File::OverwriteType::OVERWRITE : Os::File::OverwriteType::NO_OVERWRITE),
        ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenIllegalMode
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenIllegalMode::OpenIllegalMode() : Os::Test::File::Tester::AssertRule("OpenIllegalMode") {}

void Os::Test::File::Tester::OpenIllegalMode::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    std::shared_ptr<const std::string> random_filename = state.get_filename(true);
    U32 mode = STest::Pick::lowerUpper(0, 1);
    bool overwrite = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(
            state.m_file.open(random_filename->c_str(),
                              (mode == 0) ? Os::File::Mode::MAX_OPEN_MODE : Os::File::Mode::OPEN_NO_MODE,
                              overwrite ? Os::File::OverwriteType::OVERWRITE : Os::File::OverwriteType::NO_OVERWRITE),
                              Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  PreallocateIllegalOffset
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::PreallocateIllegalOffset::PreallocateIllegalOffset()
        : Os::Test::File::Tester::AssertRule("PreallocateIllegalOffset") {}

void Os::Test::File::Tester::PreallocateIllegalOffset::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    FwSignedSizeType length = static_cast<FwSignedSizeType>(STest::Pick::any());
    FwSignedSizeType invalid_offset =
            -1 * static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    ASSERT_DEATH_IF_SUPPORTED(state.m_file.preallocate(invalid_offset, length),
                              Os::Test::File::Tester::ASSERT_IN_FILE_CPP) << "With offset: " << invalid_offset;
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  PreallocateIllegalLength
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::PreallocateIllegalLength::PreallocateIllegalLength()
        : Os::Test::File::Tester::AssertRule("PreallocateIllegalLength") {}

void Os::Test::File::Tester::PreallocateIllegalLength::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    FwSignedSizeType offset = static_cast<FwSignedSizeType>(STest::Pick::any());
    FwSignedSizeType invalid_length =
            -1 * static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    ASSERT_DEATH_IF_SUPPORTED(state.m_file.preallocate(offset, invalid_length),
                              Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekIllegal
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::SeekIllegal::SeekIllegal() : Os::Test::File::Tester::AssertRule("SeekIllegal") {}

void Os::Test::File::Tester::SeekIllegal::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    ASSERT_DEATH_IF_SUPPORTED(state.m_file.seek(-1, Os::File::SeekType::ABSOLUTE), Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadIllegalBuffer
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::ReadIllegalBuffer::ReadIllegalBuffer()
        : Os::Test::File::Tester::AssertRule("ReadIllegalBuffer") {}

void Os::Test::File::Tester::ReadIllegalBuffer::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    FwSignedSizeType size = static_cast<FwSignedSizeType>(STest::Pick::any());
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(
            state.m_file.read(nullptr, size, random_wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT),
            Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadIllegalSize
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::ReadIllegalSize::ReadIllegalSize() : Os::Test::File::Tester::AssertRule("ReadIllegalSize") {}

void Os::Test::File::Tester::ReadIllegalSize::action(Os::Test::File::Tester &state  //!< The test state
) {
    U8 buffer[10] = {};
    state.assert_file_consistent();
    FwSignedSizeType invalid_size = -1 * static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(
            state.m_file.read(buffer, invalid_size,
                              random_wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT),
            Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  WriteIllegalBuffer
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::WriteIllegalBuffer::WriteIllegalBuffer()
        : Os::Test::File::Tester::AssertRule("WriteIllegalBuffer") {}

void Os::Test::File::Tester::WriteIllegalBuffer::action(Os::Test::File::Tester &state  //!< The test state
) {
    state.assert_file_consistent();
    FwSignedSizeType size = static_cast<FwSignedSizeType>(STest::Pick::any());
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(
            state.m_file.write(nullptr, size, random_wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT),
            Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  WriteIllegalSize
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::WriteIllegalSize::WriteIllegalSize() : Os::Test::File::Tester::AssertRule("WriteIllegalSize") {}

void Os::Test::File::Tester::WriteIllegalSize::action(Os::Test::File::Tester &state  //!< The test state
) {
    U8 buffer[10] = {};
    state.assert_file_consistent();
    FwSignedSizeType invalid_size = -1 * static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(
            state.m_file.read(buffer, invalid_size, random_wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT),
            Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  CopyAssignment
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::CopyAssignment::CopyAssignment() :
    STest::Rule<Os::Test::File::Tester>("CopyAssignment") {}


bool Os::Test::File::Tester::CopyAssignment::precondition(const Os::Test::File::Tester& state //!< The test state
) {
  return true;
}


void Os::Test::File::Tester::CopyAssignment::action(Os::Test::File::Tester& state //!< The test state
) {
    state.assert_file_consistent();
    Os::File temp = state.m_file;
    state.assert_file_consistent(); // Prevents optimization
    state.m_file = temp;
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  CopyConstruction
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::CopyConstruction::CopyConstruction() :
    STest::Rule<Os::Test::File::Tester>("CopyConstruction") {}


bool Os::Test::File::Tester::CopyConstruction::precondition(const Os::Test::File::Tester& state //!< The test state
) {
  return true;
}


void Os::Test::File::Tester::CopyConstruction::action(Os::Test::File::Tester& state //!< The test state
) {
    state.assert_file_consistent();
    Os::File temp(state.m_file);
    state.assert_file_consistent(); // Interim check to ensure original file did not change
    (void) new(&state.m_file)Os::File(temp); // Copy-construct overtop of the original file
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  FullCrc
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::FullCrc::FullCrc() :
    STest::Rule<Os::Test::File::Tester>("FullCrc") {}

bool Os::Test::File::Tester::FullCrc::precondition(
        const Os::Test::File::Tester& state //!< The test state
) {
    return state.m_mode == Os::File::Mode::OPEN_READ;
}


void Os::Test::File::Tester::FullCrc::action(
        Os::Test::File::Tester& state //!< The test state
) {
    U32 crc = 1;
    U32 shadow_crc = 2;
    state.assert_file_consistent();
    Os::File::Status  status = state.m_file.calculateCrc(crc);
    state.shadow_crc(shadow_crc);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    ASSERT_EQ(crc, shadow_crc);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  IncrementalCrc
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::IncrementalCrc::IncrementalCrc() :
    STest::Rule<Os::Test::File::Tester>("IncrementalCrc") {}


bool Os::Test::File::Tester::IncrementalCrc::precondition(
        const Os::Test::File::Tester& state //!< The test state
) {
    return state.m_mode == Os::File::Mode::OPEN_READ;
}


void Os::Test::File::Tester::IncrementalCrc::action(
        Os::Test::File::Tester& state //!< The test state
){
    state.assert_file_consistent();
    FwSignedSizeType size_desired = static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, FW_FILE_CHUNK_SIZE));
    FwSignedSizeType shadow_size = size_desired;
    Os::File::Status  status = state.m_file.incrementalCrc(size_desired);
    state.shadow_partial_crc(shadow_size);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    ASSERT_EQ(size_desired, shadow_size);
    ASSERT_EQ(state.m_file.m_crc, state.m_independent_crc);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  FinalizeCrc
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::FinalizeCrc::FinalizeCrc() :
    STest::Rule<Os::Test::File::Tester>("FinalizeCrc") {}

bool Os::Test::File::Tester::FinalizeCrc::precondition(
        const Os::Test::File::Tester& state //!< The test state
) {
    return true;
}

void Os::Test::File::Tester::FinalizeCrc::action(
        Os::Test::File::Tester& state //!< The test state
) {
    U32 crc = 1;
    U32 shadow_crc = 2;
    state.assert_file_consistent();
    Os::File::Status  status = state.m_file.finalizeCrc(crc);
    state.shadow_finalize(shadow_crc);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    ASSERT_EQ(crc, shadow_crc);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  FullCrcInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::FullCrcInvalidModes::FullCrcInvalidModes() :
    STest::Rule<Os::Test::File::Tester>("FullCrcInvalidModes") {}


bool Os::Test::File::Tester::FullCrcInvalidModes::precondition(
        const Os::Test::File::Tester& state //!< The test state
) {
    return Os::File::Mode::OPEN_READ != state.m_mode;
}

void Os::Test::File::Tester::FullCrcInvalidModes::action(
        Os::Test::File::Tester& state //!< The test state
) {
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_TRUE(Os::File::Mode::OPEN_READ != state.m_file.m_mode);
    U32 crc = 1;
    Os::File::Status status = state.m_file.calculateCrc(crc);
    ASSERT_EQ(crc, 0);
    state.assert_valid_mode_status(status);
    // Ensure no change in size or pointer
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  IncrementalCrcInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::IncrementalCrcInvalidModes::IncrementalCrcInvalidModes() :
    STest::Rule<Os::Test::File::Tester>("IncrementalCrcInvalidModes") {}


bool Os::Test::File::Tester::IncrementalCrcInvalidModes::precondition(
        const Os::Test::File::Tester& state //!< The test state
) {
    return Os::File::Mode::OPEN_READ != state.m_mode;;
}


void Os::Test::File::Tester::IncrementalCrcInvalidModes::action(
        Os::Test::File::Tester& state //!< The test state
) {
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_TRUE(Os::File::Mode::OPEN_READ != state.m_file.m_mode);
    FwSignedSizeType size = static_cast<FwSignedSizeType>(STest::Pick::lowerUpper(0, 1));
    Os::File::Status status = state.m_file.incrementalCrc(size);
    state.assert_valid_mode_status(status);
    // Ensure no change in size or pointer
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}

