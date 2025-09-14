// ======================================================================
// \title Os/test/ut/file/MyRules.cpp
// \brief rule implementations for common testing
// ======================================================================
#include <cstdio>
#include "RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"
extern "C" {
#include <Utils/Hash/libcrc/lib_crc.h>  // borrow CRC
}

// For testing, limit files to 32K
const FwSizeType FILE_DATA_MAXIMUM = 32 * 1024;

Os::File::Status Os::Test::FileTest::Tester::shadow_open(const std::string& path,
                                                         Os::File::Mode open_mode,
                                                         bool overwrite) {
    Os::File::Status status =
        this->m_shadow.open(path.c_str(), open_mode,
                            overwrite ? Os::File::OverwriteType::OVERWRITE : Os::File::OverwriteType::NO_OVERWRITE);
    if (Os::File::Status::OP_OK == status) {
        this->m_current_path = path;
        this->m_mode = open_mode;
        this->m_independent_crc = Os::File::INITIAL_CRC;
    } else {
        this->m_current_path.clear();
    }

    return status;
}

void Os::Test::FileTest::Tester::shadow_close() {
    this->m_shadow.close();
    this->m_current_path.clear();
    this->m_mode = Os::File::Mode::OPEN_NO_MODE;
    // Checks on the shadow data to ensure consistency
    ASSERT_TRUE(this->m_current_path.empty());
}

std::vector<U8> Os::Test::FileTest::Tester::shadow_read(FwSizeType size) {
    std::vector<U8> output;
    output.resize(size);
    Os::File::Status status = m_shadow.read(output.data(), size, Os::File::WaitType::WAIT);
    output.resize(size);
    EXPECT_EQ(status, Os::File::Status::OP_OK);
    return output;
}

void Os::Test::FileTest::Tester::shadow_write(const std::vector<U8>& write_data) {
    FwSizeType size = static_cast<FwSizeType>(write_data.size());
    FwSizeType original_size = size;
    Os::File::Status status = Os::File::OP_OK;
    if (write_data.data() != nullptr) {
        status = m_shadow.write(write_data.data(), size, Os::File::WaitType::WAIT);
    }
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    ASSERT_EQ(size, original_size);
}

void Os::Test::FileTest::Tester::shadow_seek(const FwSignedSizeType offset, const bool absolute) {
    Os::File::Status status =
        m_shadow.seek(offset, absolute ? Os::File::SeekType::ABSOLUTE : Os::File::SeekType::RELATIVE);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
}

void Os::Test::FileTest::Tester::shadow_preallocate(const FwSizeType offset, const FwSizeType length) {
    Os::File::Status status = m_shadow.preallocate(offset, length);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
}

void Os::Test::FileTest::Tester::shadow_flush() {
    Os::File::Status status = m_shadow.flush();
    ASSERT_EQ(status, Os::File::Status::OP_OK);
}

void Os::Test::FileTest::Tester::shadow_crc(U32& crc) {
    crc = this->m_independent_crc;
    SyntheticFileData& data = *reinterpret_cast<SyntheticFileData*>(this->m_shadow.getHandle());

    // Calculate CRC on full file starting at m_pointer
    for (FwSizeType i = data.m_pointer; i < data.m_data.size();
         i++, this->m_shadow.seek(1, Os::File::SeekType::RELATIVE)) {
        crc = update_crc_32(crc, static_cast<char>(data.m_data.at(i)));
    }
    // Update tracking variables
    this->m_independent_crc = Os::File::INITIAL_CRC;
}

void Os::Test::FileTest::Tester::shadow_partial_crc(FwSizeType& size) {
    SyntheticFileData data = *reinterpret_cast<SyntheticFileData*>(this->m_shadow.getHandle());

    // Calculate CRC on full file starting at m_pointer
    const FwSizeType bound = FW_MIN(static_cast<FwSizeType>(data.m_pointer) + size, data.m_data.size());
    size = (data.m_pointer >= bound) ? 0 : static_cast<FwSizeType>(bound - data.m_pointer);
    for (FwSizeType i = data.m_pointer; i < bound; i++) {
        this->m_independent_crc = update_crc_32(this->m_independent_crc, static_cast<char>(data.m_data.at(i)));
        this->m_shadow.seek(1, Os::File::SeekType::RELATIVE);
    }
}

void Os::Test::FileTest::Tester::shadow_finalize(U32& crc) {
    crc = this->m_independent_crc;
    this->m_independent_crc = Os::File::INITIAL_CRC;
}

Os::Test::FileTest::Tester::FileState Os::Test::FileTest::Tester::current_file_state() {
    Os::Test::FileTest::Tester::FileState state;
    // Invariant: mode must not be closed, or path must be nullptr
    EXPECT_TRUE((Os::File::Mode::OPEN_NO_MODE != this->m_file.m_mode) || (nullptr == this->m_file.m_path));

    // Read state when file is open
    if (Os::File::Mode::OPEN_NO_MODE != this->m_file.m_mode) {
        EXPECT_EQ(this->m_file.position(state.position), Os::File::Status::OP_OK);
        EXPECT_EQ(this->m_file.size(state.size), Os::File::Status::OP_OK);
        // Extra check to ensure size does not alter pointer
        FwSizeType new_position = std::numeric_limits<FwSizeType>::max();
        EXPECT_EQ(this->m_file.position(new_position), Os::File::Status::OP_OK);
        EXPECT_EQ(new_position, state.position);
    }
    return state;
}

void Os::Test::FileTest::Tester::assert_valid_mode_status(Os::File::Status& status) const {
    if (Os::File::Mode::OPEN_NO_MODE == this->m_mode) {
        ASSERT_EQ(status, Os::File::Status::NOT_OPENED);
    } else {
        ASSERT_EQ(status, Os::File::Status::INVALID_MODE);
    }
}

void Os::Test::FileTest::Tester::assert_file_consistent() {
    // Ensure file mode
    ASSERT_EQ(this->m_mode, this->m_file.m_mode);
    // Ensure CRC match
    ASSERT_EQ(this->m_file.m_crc, this->m_independent_crc);
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
                FwSizeType current_position = 0;
                FwSizeType shadow_position = 0;
                ASSERT_EQ(this->m_file.position(current_position), Os::File::Status::OP_OK);
                ASSERT_EQ(this->m_shadow.position(shadow_position), Os::File::Status::OP_OK);

                ASSERT_EQ(current_position, shadow_position);
                // Ensure the file size is consistent
                FwSizeType current_size = 0;
                FwSizeType shadow_size = 0;
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

void Os::Test::FileTest::Tester::assert_file_opened(const std::string& path,
                                                    Os::File::Mode newly_opened_mode,
                                                    bool overwrite) {
    // Assert the that the file is opened in some mode
    ASSERT_NE(this->m_file.m_mode, Os::File::Mode::OPEN_NO_MODE);
    ASSERT_TRUE(this->m_file.isOpen()) << "`isOpen()` failed to indicate file is open";
    ASSERT_EQ(this->m_file.m_mode, this->m_mode);

    // When the open mode has been specified assert that is in an exact state
    if (not path.empty() && Os::File::Mode::OPEN_NO_MODE != newly_opened_mode) {
        // Assert file pointer always at beginning when functional
        if (functional()) {
            FwSizeType file_position = std::numeric_limits<FwSizeType>::max();
            ASSERT_EQ(this->m_file.position(file_position), Os::File::Status::OP_OK);
            ASSERT_EQ(file_position, 0);
        }
        ASSERT_EQ(std::string(this->m_file.m_path), path);
        ASSERT_EQ(this->m_file.m_mode, newly_opened_mode) << "File is in unexpected mode";

        // Check truncations
        const bool truncate = (Os::File::Mode::OPEN_CREATE == newly_opened_mode) && overwrite;
        if (truncate) {
            if (this->functional()) {
                FwSizeType file_size = std::numeric_limits<FwSizeType>::max();
                ASSERT_EQ(this->m_file.size(file_size), Os::File::Status::OP_OK);
                ASSERT_EQ(file_size, 0);
            }
        }
    }
}

void Os::Test::FileTest::Tester::assert_file_closed() {
    ASSERT_EQ(this->m_file.m_mode, Os::File::Mode::OPEN_NO_MODE) << "File is in unexpected mode";
    ASSERT_FALSE(this->m_file.isOpen()) << "`isOpen()` failed to indicate file is open";
}

void Os::Test::FileTest::Tester::assert_file_read(const std::vector<U8>& state_data,
                                                  const unsigned char* read_data,
                                                  FwSizeType size_read) {
    // Functional tests
    if (functional()) {
        ASSERT_EQ(size_read, state_data.size());
        ASSERT_EQ(std::vector<U8>(read_data, read_data + size_read), state_data);
        FwSizeType position = std::numeric_limits<FwSizeType>::max();
        FwSizeType shadow_position = std::numeric_limits<FwSizeType>::max();
        ASSERT_EQ(this->m_file.position(position), Os::File::Status::OP_OK);
        ASSERT_EQ(this->m_shadow.position(shadow_position), Os::File::Status::OP_OK);
        ASSERT_EQ(position, shadow_position);
    }
}

void Os::Test::FileTest::Tester::assert_file_write(const std::vector<U8>& write_data, FwSizeType size_written) {
    ASSERT_EQ(size_written, write_data.size());
    FwSizeType file_size = 0;
    FwSizeType shadow_size = 0;
    ASSERT_EQ(this->m_file.size(file_size), Os::File::Status::OP_OK);
    ASSERT_EQ(this->m_shadow.size(shadow_size), Os::File::Status::OP_OK);
    ASSERT_EQ(file_size, shadow_size);
    FwSizeType file_position = std::numeric_limits<FwSizeType>::max();
    FwSizeType shadow_position = std::numeric_limits<FwSizeType>::max();
    ASSERT_EQ(this->m_file.position(file_position), Os::File::Status::OP_OK);
    ASSERT_EQ(this->m_shadow.position(shadow_position), Os::File::Status::OP_OK);
    ASSERT_EQ(file_position, shadow_position);
}

void Os::Test::FileTest::Tester::assert_file_seek(const FwSizeType original_position,
                                                  const FwSignedSizeType seek_desired,
                                                  const bool absolute) {
    FwSizeType new_position = 0;
    FwSizeType shadow_position = 0;

    ASSERT_EQ(this->m_file.position(new_position), Os::File::Status::OP_OK);
    ASSERT_EQ(this->m_shadow.position(shadow_position), Os::File::Status::OP_OK);

    const FwSignedSizeType expected_offset = (absolute) ? seek_desired : (original_position + seek_desired);
    if (expected_offset >= 0) {
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
Os::Test::FileTest::Tester::OpenBaseRule::OpenBaseRule(const char* rule_name,
                                                       Os::File::Mode mode,
                                                       const bool overwrite,
                                                       const bool randomize_filename)
    : STest::Rule<Os::Test::FileTest::Tester>(rule_name),
      m_mode(mode),
      m_overwrite(overwrite ? Os::File::OverwriteType::OVERWRITE : Os::File::OverwriteType::NO_OVERWRITE),
      m_random(randomize_filename) {}

bool Os::Test::FileTest::Tester::OpenBaseRule::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return state.m_mode == Os::File::Mode::OPEN_NO_MODE;
}

void Os::Test::FileTest::Tester::OpenBaseRule::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s mode %d\n", this->getName(), this->m_mode);
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
        ASSERT_EQ(file_state.position, 0);  // Open always zeros the position
    }
    // Assert the file state remains consistent.
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenFileCreate
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::OpenFileCreate::OpenFileCreate(const bool randomize_filename)
    : Os::Test::FileTest::Tester::OpenBaseRule("OpenFileCreate",
                                               Os::File::Mode::OPEN_CREATE,
                                               false,
                                               randomize_filename) {}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenFileCreateOverwrite
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::OpenFileCreateOverwrite::OpenFileCreateOverwrite(const bool randomize_filename)
    : Os::Test::FileTest::Tester::OpenBaseRule("OpenFileCreate",
                                               Os::File::Mode::OPEN_CREATE,
                                               true,
                                               randomize_filename) {}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForWrite
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::OpenForWrite::OpenForWrite(const bool randomize_filename)
    : Os::Test::FileTest::Tester::OpenBaseRule(
          "OpenForWrite",
          // Randomized write mode
          static_cast<Os::File::Mode>(STest::Pick::lowerUpper(Os::File::Mode::OPEN_WRITE, Os::File::Mode::OPEN_APPEND)),
          // Randomized overwrite
          static_cast<bool>(STest::Pick::lowerUpper(0, 1)),
          randomize_filename) {
    // Ensures that a random write mode will work correctly
    static_assert((Os::File::Mode::OPEN_SYNC_WRITE - 1) == Os::File::Mode::OPEN_WRITE, "Write modes not contiguous");
    static_assert((Os::File::Mode::OPEN_APPEND - 1) == Os::File::Mode::OPEN_SYNC_WRITE, "Write modes not contiguous");
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForAppend
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::OpenForAppend::OpenForAppend(const bool randomize_filename)
    : Os::Test::FileTest::Tester::OpenBaseRule("OpenForAppend",
                                               // Randomized write mode
                                               Os::File::Mode::OPEN_APPEND,
                                               // Randomized overwrite
                                               false,
                                               randomize_filename) {
    // Ensures that a random write mode will work correctly
    static_assert((Os::File::Mode::OPEN_SYNC_WRITE - 1) == Os::File::Mode::OPEN_WRITE, "Write modes not contiguous");
    static_assert((Os::File::Mode::OPEN_APPEND - 1) == Os::File::Mode::OPEN_SYNC_WRITE, "Write modes not contiguous");
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForRead
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::OpenForRead::OpenForRead(const bool randomize_filename)
    : Os::Test::FileTest::Tester::OpenBaseRule("OpenForRead",
                                               Os::File::Mode::OPEN_READ,
                                               // Randomized overwrite
                                               static_cast<bool>(STest::Pick::lowerUpper(0, 1)),
                                               randomize_filename) {}

// ------------------------------------------------------------------------------------------------------
// Rule:  CloseFile
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::CloseFile::CloseFile() : STest::Rule<Os::Test::FileTest::Tester>("CloseFile") {}

bool Os::Test::FileTest::Tester::CloseFile::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE != state.m_mode;
}

void Os::Test::FileTest::Tester::CloseFile::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
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

Os::Test::FileTest::Tester::Read::Read() : STest::Rule<Os::Test::FileTest::Tester>("Read") {}

bool Os::Test::FileTest::Tester::Read::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_READ == state.m_mode;
}

void Os::Test::FileTest::Tester::Read::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    U8 buffer[FILE_DATA_MAXIMUM];
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    FwSizeType size_desired = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM));
    FwSizeType size_read = size_desired;
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    Os::File::Status status =
        state.m_file.read(buffer, size_read, wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT);
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

Os::Test::FileTest::Tester::Write::Write() : STest::Rule<Os::Test::FileTest::Tester>("Write") {}

bool Os::Test::FileTest::Tester::Write::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_CREATE <= state.m_mode;
}

void Os::Test::FileTest::Tester::Write::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    U8 buffer[FILE_DATA_MAXIMUM];
    state.assert_file_consistent();
    FwSizeType current_position = 0;
    state.m_file.position(current_position);
    if (state.m_mode == Os::File::Mode::OPEN_APPEND) {
        state.m_file.size(current_position);
    }
    FwSizeType size_desired = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM - current_position));
    FwSizeType size_written = size_desired;
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    for (FwSizeType i = 0; i < size_desired; i++) {
        buffer[i] = static_cast<U8>(STest::Pick::lowerUpper(0, std::numeric_limits<U8>::max()));
    }
    std::vector<U8> write_data(buffer, buffer + size_desired);
    Os::File::Status status =
        state.m_file.write(buffer, size_written, wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT);
    ASSERT_EQ(Os::File::Status::OP_OK, status);
    ASSERT_EQ(size_written, size_desired);
    state.shadow_write(write_data);
    state.assert_file_write(write_data, size_written);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Seek
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::Seek::Seek() : STest::Rule<Os::Test::FileTest::Tester>("Seek") {}

bool Os::Test::FileTest::Tester::Seek::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE < state.m_mode;
}

void Os::Test::FileTest::Tester::Seek::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    FwSignedSizeType seek_offset = 0;
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();

    // Choose some random values
    bool absolute = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    if (absolute) {
        seek_offset = STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM);
    } else {
        seek_offset = STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM);
        seek_offset -= original_file_state.position;
    }
    Os::File::Status status =
        state.m_file.seek(seek_offset, absolute ? Os::File::SeekType::ABSOLUTE : Os::File::SeekType::RELATIVE);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    state.shadow_seek(seek_offset, absolute);
    state.assert_file_seek(original_file_state.position, seek_offset, absolute);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Preallocate
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::Preallocate::Preallocate() : STest::Rule<Os::Test::FileTest::Tester>("Preallocate") {}

bool Os::Test::FileTest::Tester::Preallocate::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_CREATE <= state.m_mode;
}

void Os::Test::FileTest::Tester::Preallocate::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    FwSizeType offset = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM - 1));
    FwSizeType length = static_cast<FwSizeType>(STest::Pick::lowerUpper(1, FILE_DATA_MAXIMUM - offset));
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

Os::Test::FileTest::Tester::Flush::Flush() : STest::Rule<Os::Test::FileTest::Tester>("Flush") {}

bool Os::Test::FileTest::Tester::Flush::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_CREATE <= state.m_mode;
}

void Os::Test::FileTest::Tester::Flush::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
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

Os::Test::FileTest::Tester::OpenInvalidModes::OpenInvalidModes()
    : STest::Rule<Os::Test::FileTest::Tester>("OpenInvalidModes") {}

bool Os::Test::FileTest::Tester::OpenInvalidModes::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE != state.m_mode;
}

void Os::Test::FileTest::Tester::OpenInvalidModes::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
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

Os::Test::FileTest::Tester::PreallocateWithoutOpen::PreallocateWithoutOpen()
    : STest::Rule<Os::Test::FileTest::Tester>("PreallocateWithoutOpen") {}

bool Os::Test::FileTest::Tester::PreallocateWithoutOpen::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.m_mode;
}

void Os::Test::FileTest::Tester::PreallocateWithoutOpen::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    FwSizeType random_offset = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());
    FwSizeType random_size = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());

    Os::File::Status status = state.m_file.preallocate(random_offset, random_size);
    state.assert_valid_mode_status(status);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekWithoutOpen
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::SeekWithoutOpen::SeekWithoutOpen()
    : STest::Rule<Os::Test::FileTest::Tester>("SeekWithoutOpen") {}

bool Os::Test::FileTest::Tester::SeekWithoutOpen::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.m_mode;
}

void Os::Test::FileTest::Tester::SeekWithoutOpen::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    FwSignedSizeType random_offset = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());
    bool random_absolute = static_cast<bool>(STest::Pick::lowerUpper(0, 1));

    Os::File::Status status =
        state.m_file.seek(random_offset, random_absolute ? Os::File::SeekType::ABSOLUTE : Os::File::SeekType::RELATIVE);
    state.assert_valid_mode_status(status);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekInvalidSize
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::SeekInvalidSize::SeekInvalidSize()
    : STest::Rule<Os::Test::FileTest::Tester>("SeekInvalidSize") {}

bool Os::Test::FileTest::Tester::SeekInvalidSize::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    FwSizeType position = 0;
    // Operation is effectively constant
    Os::File::Status status = const_cast<Os::Test::FileTest::Tester&>(state).m_file.position(position);
    return (Os::File::Mode::OPEN_NO_MODE < state.m_mode) &&
           // Limitation of the test harness: max random value is U32_MAX, thus we need at least 1 byte headroom
           (status == Os::File::Status::OP_OK) && (position < std::numeric_limits<U32>::max());
}

void Os::Test::FileTest::Tester::SeekInvalidSize::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    // Open file of given filename
    FwSignedSizeType random_offset =
        STest::Pick::lowerUpper(original_file_state.position + 1, std::numeric_limits<U32>::max());
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

Os::Test::FileTest::Tester::FlushInvalidModes::FlushInvalidModes()
    : STest::Rule<Os::Test::FileTest::Tester>("FlushInvalidModes") {}

bool Os::Test::FileTest::Tester::FlushInvalidModes::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.m_mode || Os::File::Mode::OPEN_READ == state.m_mode;
}

void Os::Test::FileTest::Tester::FlushInvalidModes::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_TRUE(Os::File::Mode::OPEN_NO_MODE == state.m_file.m_mode ||
                Os::File::Mode::OPEN_READ == state.m_file.m_mode);
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

Os::Test::FileTest::Tester::ReadInvalidModes::ReadInvalidModes()
    : STest::Rule<Os::Test::FileTest::Tester>("ReadInvalidModes") {}

bool Os::Test::FileTest::Tester::ReadInvalidModes::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_READ != state.m_mode;
}

void Os::Test::FileTest::Tester::ReadInvalidModes::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    U8 buffer[10];
    FwSizeType size = sizeof buffer;
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_NE(Os::File::Mode::OPEN_READ, state.m_file.m_mode);
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    Os::File::Status status =
        state.m_file.read(buffer, size, wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT);
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

Os::Test::FileTest::Tester::WriteInvalidModes::WriteInvalidModes()
    : STest::Rule<Os::Test::FileTest::Tester>("WriteInvalidModes") {}

bool Os::Test::FileTest::Tester::WriteInvalidModes::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_NO_MODE == state.m_mode || Os::File::Mode::OPEN_READ == state.m_mode;
}

void Os::Test::FileTest::Tester::WriteInvalidModes::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    U8 buffer[10];
    FwSizeType size = sizeof buffer;
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_TRUE(Os::File::Mode::OPEN_NO_MODE == state.m_file.m_mode ||
                Os::File::Mode::OPEN_READ == state.m_file.m_mode);
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    Os::File::Status status =
        state.m_file.write(buffer, size, wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT);
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

Os::Test::FileTest::Tester::AssertRule::AssertRule(const char* name) : STest::Rule<Os::Test::FileTest::Tester>(name) {}

bool Os::Test::FileTest::Tester::AssertRule::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return true;
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenIllegalPath
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::OpenIllegalPath::OpenIllegalPath()
    : Os::Test::FileTest::Tester::AssertRule("OpenIllegalPath") {}

void Os::Test::FileTest::Tester::OpenIllegalPath::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    Os::File::Mode random_mode =
        static_cast<Os::File::Mode>(STest::Pick::lowerUpper(Os::File::Mode::OPEN_READ, Os::File::Mode::OPEN_APPEND));
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

Os::Test::FileTest::Tester::OpenIllegalMode::OpenIllegalMode()
    : Os::Test::FileTest::Tester::AssertRule("OpenIllegalMode") {}

void Os::Test::FileTest::Tester::OpenIllegalMode::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    std::shared_ptr<const std::string> random_filename = state.get_filename(true);
    U32 mode = STest::Pick::lowerUpper(0, 1);
    bool overwrite = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(
        state.m_file.open(random_filename->c_str(),
                          (mode == 0) ? Os::File::Mode::MAX_OPEN_MODE : Os::File::Mode::OPEN_NO_MODE,
                          overwrite ? Os::File::OverwriteType::OVERWRITE : Os::File::OverwriteType::NO_OVERWRITE),
        Os::Test::FileTest::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekIllegal
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::SeekIllegal::SeekIllegal() : Os::Test::FileTest::Tester::AssertRule("SeekIllegal") {}

void Os::Test::FileTest::Tester::SeekIllegal::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    ASSERT_DEATH_IF_SUPPORTED(state.m_file.seek(-1, Os::File::SeekType::ABSOLUTE),
                              Os::Test::FileTest::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadIllegalBuffer
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::ReadIllegalBuffer::ReadIllegalBuffer()
    : Os::Test::FileTest::Tester::AssertRule("ReadIllegalBuffer") {}

void Os::Test::FileTest::Tester::ReadIllegalBuffer::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    FwSizeType size = static_cast<FwSizeType>(STest::Pick::any());
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(
        state.m_file.read(nullptr, size, random_wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT),
        Os::Test::FileTest::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  WriteIllegalBuffer
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::WriteIllegalBuffer::WriteIllegalBuffer()
    : Os::Test::FileTest::Tester::AssertRule("WriteIllegalBuffer") {}

void Os::Test::FileTest::Tester::WriteIllegalBuffer::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    FwSizeType size = static_cast<FwSizeType>(STest::Pick::any());
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(
        state.m_file.write(nullptr, size, random_wait ? Os::File::WaitType::WAIT : Os::File::WaitType::NO_WAIT),
        Os::Test::FileTest::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  CopyAssignment
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::CopyAssignment::CopyAssignment()
    : STest::Rule<Os::Test::FileTest::Tester>("CopyAssignment") {}

bool Os::Test::FileTest::Tester::CopyAssignment::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return true;
}

void Os::Test::FileTest::Tester::CopyAssignment::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    Os::File temp = state.m_file;
    state.assert_file_consistent();  // Prevents optimization
    state.m_file = temp;
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  CopyConstruction
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::CopyConstruction::CopyConstruction()
    : STest::Rule<Os::Test::FileTest::Tester>("CopyConstruction") {}

bool Os::Test::FileTest::Tester::CopyConstruction::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return true;
}

void Os::Test::FileTest::Tester::CopyConstruction::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    Os::File temp(state.m_file);
    state.assert_file_consistent();            // Interim check to ensure original file did not change
    (void)new (&state.m_file) Os::File(temp);  // Copy-construct overtop of the original file
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  FullCrc
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::FullCrc::FullCrc() : STest::Rule<Os::Test::FileTest::Tester>("FullCrc") {}

bool Os::Test::FileTest::Tester::FullCrc::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return state.m_mode == Os::File::Mode::OPEN_READ;
}

void Os::Test::FileTest::Tester::FullCrc::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    U32 crc = 1;
    U32 shadow_crc = 2;
    state.assert_file_consistent();
    Os::File::Status status = state.m_file.calculateCrc(crc);
    state.shadow_crc(shadow_crc);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    ASSERT_EQ(crc, shadow_crc);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  IncrementalCrc
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::IncrementalCrc::IncrementalCrc()
    : STest::Rule<Os::Test::FileTest::Tester>("IncrementalCrc") {}

bool Os::Test::FileTest::Tester::IncrementalCrc::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return state.m_mode == Os::File::Mode::OPEN_READ;
}

void Os::Test::FileTest::Tester::IncrementalCrc::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    FwSizeType size_desired = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, FW_FILE_CHUNK_SIZE));
    FwSizeType shadow_size = size_desired;
    Os::File::Status status = state.m_file.incrementalCrc(size_desired);
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

Os::Test::FileTest::Tester::FinalizeCrc::FinalizeCrc() : STest::Rule<Os::Test::FileTest::Tester>("FinalizeCrc") {}

bool Os::Test::FileTest::Tester::FinalizeCrc::precondition(const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return true;
}

void Os::Test::FileTest::Tester::FinalizeCrc::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    U32 crc = 1;
    U32 shadow_crc = 2;
    state.assert_file_consistent();
    Os::File::Status status = state.m_file.finalizeCrc(crc);
    state.shadow_finalize(shadow_crc);
    ASSERT_EQ(status, Os::File::Status::OP_OK);
    ASSERT_EQ(crc, shadow_crc);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  FullCrcInvalidModes
//
// ------------------------------------------------------------------------------------------------------

Os::Test::FileTest::Tester::FullCrcInvalidModes::FullCrcInvalidModes()
    : STest::Rule<Os::Test::FileTest::Tester>("FullCrcInvalidModes") {}

bool Os::Test::FileTest::Tester::FullCrcInvalidModes::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_READ != state.m_mode;
}

void Os::Test::FileTest::Tester::FullCrcInvalidModes::action(Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
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

Os::Test::FileTest::Tester::IncrementalCrcInvalidModes::IncrementalCrcInvalidModes()
    : STest::Rule<Os::Test::FileTest::Tester>("IncrementalCrcInvalidModes") {}

bool Os::Test::FileTest::Tester::IncrementalCrcInvalidModes::precondition(
    const Os::Test::FileTest::Tester& state  //!< The test state
) {
    return Os::File::Mode::OPEN_READ != state.m_mode;
}

void Os::Test::FileTest::Tester::IncrementalCrcInvalidModes::action(
    Os::Test::FileTest::Tester& state  //!< The test state
) {
    printf("--> Rule: %s \n", this->getName());
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_TRUE(Os::File::Mode::OPEN_READ != state.m_file.m_mode);
    FwSizeType size = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, 1));
    Os::File::Status status = state.m_file.incrementalCrc(size);
    state.assert_valid_mode_status(status);
    // Ensure no change in size or pointer
    FileState final_file_state = state.current_file_state();
    ASSERT_EQ(final_file_state.size, original_file_state.size);
    ASSERT_EQ(final_file_state.position, original_file_state.position);
    state.assert_file_consistent();
}
