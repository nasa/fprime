

#include "RulesHeaders.hpp"
#include "STest/Pick/Pick.hpp"

// For testing, limit files to 32K
const FwSizeType FILE_DATA_MAXIMUM = 32 * 1024;

Os::File::Status Os::Test::File::Tester::shadow_open(const std::string &path, Os::File::Mode open_mode, bool overwrite) {
    Os::File::Status status = Os::File::Status::OP_OK;
    bool exists = (this->filesystem.count(path) > 0);

    // Error case: read on file that does not exist
    if ((not exists) && (Os::File::Mode::OPEN_READ == open_mode)) {
        status = Os::File::Status::DOESNT_EXIST;
        this->current_path.clear();
        this->mode = Os::File::Mode::OPEN_NO_MODE;
    }
    // Error case: create on existing file without overwrite
    else if (exists and (not overwrite) && (open_mode == Os::File::Mode::OPEN_CREATE)) {
        status = Os::File::Status::FILE_EXISTS;
        this->current_path.clear();
        this->mode = Os::File::Mode::OPEN_NO_MODE;
    }
    // Case where file should be opened correctly
    else {
        const bool truncate = (Os::File::Mode::OPEN_CREATE == open_mode) && overwrite;
        this->current_path = path;
        this->mode = open_mode;

        // Add new shadow file data when the file is new
        if (not exists) {
            this->filesystem[this->current_path] = std::unique_ptr<FileData>(new FileData);
        }
        // Update the shadow state to look like a newly opened file
        FileData &data = *this->filesystem.at(this->current_path);
        if (truncate) {
            data.data.clear();
        }
        data.pointer = 0;
        // Checks on the shadow data to ensure consistency
        EXPECT_NE(this->mode, Os::File::OPEN_NO_MODE);
        EXPECT_EQ(data.pointer, 0);
        EXPECT_EQ(this->current_path, path);
        EXPECT_TRUE(not truncate || data.data.empty());
    }
    return status;
}

void Os::Test::File::Tester::shadow_close() {
    this->mode = Os::File::Mode::OPEN_NO_MODE;
    this->current_path.clear();
    // Checks on the shadow data to ensure consistency
    ASSERT_EQ(this->mode, Os::File::Mode::OPEN_NO_MODE);
    ASSERT_TRUE(this->current_path.empty());
}

std::vector<U8> Os::Test::File::Tester::shadow_read(FwSizeType size) {
    std::vector<U8> output;
    FileData &data = *filesystem.at(this->current_path);
    FwSizeType original_pointer = data.pointer;
    FwSizeType original_size = data.data.size();
    // Check expected read bytes
    for (FwSizeType i = 0; i < size; i++, data.pointer++) {
        // End of file
        if (data.pointer >= static_cast<FwSizeType>(data.data.size())) {
            break;
        }
        output.push_back(data.data.at(data.pointer));
    }
    // Checks on the shadow data to ensure consistency
    EXPECT_EQ(data.data.size(), original_size);
    EXPECT_EQ(data.pointer, (original_pointer > original_size) ? original_pointer : FW_MIN(original_pointer + size, original_size));
    EXPECT_EQ(output.size(), (original_pointer > original_size) ? 0 : FW_MIN(size, original_size - original_pointer));
    return output;
}

void Os::Test::File::Tester::shadow_write(const std::vector<U8>& write_data) {
    FileData &data = *filesystem.at(this->current_path);
    FwSizeType original_position = data.pointer;
    FwSizeType original_size = data.data.size();

    // Appends seek to end before writing
    if (Os::File::Mode::OPEN_APPEND == this->mode) {
        data.pointer = data.data.size();
    }

    // First add in zeros to account for a pointer past the end of the file
    const FwSizeType zeros = static_cast<FwSizeType>(data.pointer) - static_cast<FwSizeType>(data.data.size());
    for (FwSizeType i = 0; i < zeros; i++) {
        data.data.push_back(0);
    }
    // Interim checks to ensure zeroing performed correctly
    ASSERT_LE(data.pointer, data.data.size());
    ASSERT_EQ(data.data.size(),
              ((Os::File::Mode::OPEN_APPEND == this->mode) ? original_size : FW_MAX(original_position, original_size)));

    FwSizeType pre_write_position = data.pointer;
    FwSizeType pre_write_size = data.data.size();

    // Next write data
    for (FwSizeType i = 0; i < static_cast<FwSizeType>(write_data.size()); i++, data.pointer++) {
        // Overwrite case
        if (data.pointer < static_cast<FwSizeType>(data.data.size())) {
            data.data.at(data.pointer) = write_data[i];
        }
        // Append case
        else {
            data.data.push_back(write_data[i]);
        }
    }
    // Checks on the shadow data to ensure consistency
    EXPECT_EQ(data.data.size(),
              FW_MAX(pre_write_position + static_cast<FwSizeType>(write_data.size()), pre_write_size));
    EXPECT_EQ(data.pointer,
              ((Os::File::Mode::OPEN_APPEND == this->mode) ? pre_write_size : pre_write_position) + write_data.size());
}

void Os::Test::File::Tester::shadow_seek(const FwSizeType offset, const bool absolute) {
    FileData &data = *filesystem.at(this->current_path);
    FwSizeType new_offset = (absolute) ? offset : (offset + data.pointer);
    ASSERT_GE(new_offset, 0);
    data.pointer = new_offset;
}

void Os::Test::File::Tester::shadow_preallocate(const FwSizeType offset, const FwSizeType length) {
    FileData& data = *this->filesystem.at(this->current_path);
    const FwSizeType original_size = data.data.size();
    const FwSizeType new_length = offset + length;
    // Loop from existing size to new size adding zeros
    for (FwSizeType i = static_cast<FwSizeType>(data.data.size()); i < new_length; i++) {
        data.data.push_back(0);
    }
    ASSERT_EQ(data.data.size(), FW_MAX(offset + length, original_size));
}

void Os::Test::File::Tester::shadow_flush() {
    // Flush does nothing, function kept as pattern placeholder
}

Os::Test::File::Tester::FileState Os::Test::File::Tester::current_file_state() {
    Os::Test::File::Tester::FileState state;
    // Invariant: mode must not be closed, or path must be nullptr
    EXPECT_TRUE((Os::File::Mode::OPEN_NO_MODE != this->file.m_mode) || (nullptr == file.m_path));

    // Read state when file is open
    if (Os::File::Mode::OPEN_NO_MODE != this->file.m_mode) {
        EXPECT_EQ(this->file.position(state.position), Os::File::Status::OP_OK);
        EXPECT_EQ(this->file.size(state.size), Os::File::Status::OP_OK);
        // Extra check to ensure size does not alter pointer
        FwSizeType new_position = -1;
        EXPECT_EQ(this->file.position(new_position), Os::File::Status::OP_OK);
        EXPECT_EQ(new_position, state.position);
    }
    return state;
}

void Os::Test::File::Tester::assert_valid_mode_status(Os::File::Status &status) const {
    if (Os::File::Mode::OPEN_NO_MODE == this->mode) {
        ASSERT_EQ(status, Os::File::Status::NOT_OPENED);
    } else {
        ASSERT_EQ(status, Os::File::Status::INVALID_MODE);
    }
}


void Os::Test::File::Tester::assert_file_consistent() {
    // Ensure file mode
    ASSERT_EQ(this->mode, this->file.m_mode);
    if (this->file.m_path == nullptr) {
        ASSERT_EQ(this->current_path, std::string(""));
    } else {
        // Ensure the state path matches the file path
        std::string path = std::string(this->file.m_path);
        ASSERT_EQ(path, this->current_path);

        // Check real file properties when able to do so
        if (this->functional()) {
            //  File exists, check all properties
            if (this->filesystem.count(this->current_path) > 0) {
                ASSERT_EQ(this->filesystem.count(this->current_path), 1);
                FileData &data = *this->filesystem.at(this->current_path);
                // Ensure the file pointer is consistent
                FwSizeType current_position = 0;
                ASSERT_EQ(this->file.position(current_position), Os::File::Status::OP_OK);
                ASSERT_EQ(current_position, data.pointer);
                // Ensure the file size is consistent
                FwSizeType current_size = 0;
                ASSERT_EQ(this->file.size(current_size), Os::File::Status::OP_OK);
                ASSERT_EQ(current_size, data.data.size());
            }
                // Does not exist
            else {
                ASSERT_FALSE(this->exists(this->current_path));
            }
        }
    }
}

void Os::Test::File::Tester::assert_file_opened(const std::string &path, Os::File::Mode newly_opened_mode, bool overwrite) {
    // Assert the that the file is opened in some mode
    ASSERT_NE(this->file.m_mode, Os::File::Mode::OPEN_NO_MODE);
    ASSERT_TRUE(this->file.isOpen()) << "`isOpen()` failed to indicate file is open";
    ASSERT_EQ(this->file.m_mode, this->mode);

    // When the open mode has been specified assert that is in an exact state
    if (not path.empty() && Os::File::Mode::OPEN_NO_MODE != newly_opened_mode) {
        // Assert file pointer always at beginning when functional
        if (functional() ) {
            FwSizeType file_position = -1;
            ASSERT_EQ(this->file.position(file_position), Os::File::Status::OP_OK);
            ASSERT_EQ(file_position, 0);
        }
        ASSERT_EQ(std::string(this->file.m_path), path);
        ASSERT_EQ(this->file.m_mode, newly_opened_mode) << "File is in unexpected mode";

        // Check truncations
        const bool truncate = (Os::File::Mode::OPEN_CREATE == newly_opened_mode) && overwrite;
        if (truncate) {
            if (this->functional()) {
                FwSizeType file_size = -1;
                ASSERT_EQ(this->file.size(file_size), Os::File::Status::OP_OK);
                ASSERT_EQ(file_size, 0);
            }
        }
    }
}

void Os::Test::File::Tester::assert_file_closed() {
    ASSERT_EQ(this->file.m_mode, Os::File::Mode::OPEN_NO_MODE) << "File is in unexpected mode";
    ASSERT_FALSE(this->file.isOpen()) << "`isOpen()` failed to indicate file is open";
}

void Os::Test::File::Tester::assert_file_read(const std::vector<U8>& state_data, const unsigned char *read_data, FwSizeType size_read) {
    FileData &data = *this->filesystem.at(this->current_path);
    // Functional tests
    if (functional()) {
        ASSERT_EQ(size_read, state_data.size());
        ASSERT_EQ(std::vector<U8>(read_data, read_data + size_read), state_data);
        FwSizeType position = -1;
        ASSERT_EQ(this->file.position(position), Os::File::Status::OP_OK);
        ASSERT_EQ(position, data.pointer);
    }
}

void Os::Test::File::Tester::assert_file_write(const std::vector<U8>& write_data, FwSizeType size_written) {
    FileData &data = *this->filesystem.at(this->current_path);
    ASSERT_EQ(size_written, write_data.size());
    FwSizeType file_size = 0;
    ASSERT_EQ(this->file.size(file_size), Os::File::Status::OP_OK);
    ASSERT_EQ(static_cast<FwSizeType>(data.data.size()), file_size);
    FwSizeType file_position = -1;
    ASSERT_EQ(this->file.position(file_position), Os::File::Status::OP_OK);
    ASSERT_EQ(file_position, data.pointer);
}

void Os::Test::File::Tester::assert_file_seek(const FwSizeType original_position, const FwSizeType seek_desired, const bool absolute) {
    FileData &data = *this->filesystem.at(this->current_path);
    FwSizeType new_position = 0;
    ASSERT_EQ(this->file.position(new_position), Os::File::Status::OP_OK);

    const FwSizeType expected_offset = (absolute) ? seek_desired : (original_position + seek_desired);
    if (expected_offset > 0) {
        ASSERT_EQ(new_position, expected_offset);
    } else {
        ASSERT_EQ(new_position, original_position);
    }
    ASSERT_EQ(data.pointer, new_position);
}

// ------------------------------------------------------------------------------------------------------
//  OpenFile: base rule for all open rules
//
// ------------------------------------------------------------------------------------------------------
Os::Test::File::Tester::OpenBaseRule::OpenBaseRule(const char *rule_name,
                                                   Os::File::Mode mode,
                                                   const bool overwrite,
                                                   const bool randomize_filename)
        : STest::Rule<Os::Test::File::Tester>(rule_name), m_mode(mode), m_overwrite(overwrite),
          m_random(randomize_filename) {}

bool Os::Test::File::Tester::OpenBaseRule::precondition(const Os::Test::File::Tester &state  //!< The test state
) {
    return state.mode == Os::File::Mode::OPEN_NO_MODE;
}

void Os::Test::File::Tester::OpenBaseRule::action(Os::Test::File::Tester &state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    // Initial variables used for this test
    std::shared_ptr<const std::string> filename = state.get_filename(this->m_random);

    // Ensure initial and shadow states synchronized
    state.assert_file_consistent();
    state.assert_file_closed();

    // Perform action and shadow action asserting the results are the same
    Os::File::Status status = state.file.open(filename->c_str(), m_mode, this->m_overwrite);
    ASSERT_EQ(status, state.shadow_open(*filename, m_mode, this->m_overwrite));

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
    static_assert((Os::File::Mode::OPEN_SYNC_DIRECT_WRITE - 1) == Os::File::Mode::OPEN_SYNC_WRITE,
                  "Write modes not contiguous");
    static_assert((Os::File::Mode::OPEN_APPEND - 1) == Os::File::Mode::OPEN_SYNC_DIRECT_WRITE,
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
    return Os::File::Mode::OPEN_NO_MODE != state.mode;
}

void Os::Test::File::Tester::CloseFile::action(Os::Test::File::Tester &state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    // Make sure test state and file state synchronized
    state.assert_file_consistent();
    state.assert_file_opened(state.current_path);
    // Close file and shadow state
    state.file.close();
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
    return Os::File::Mode::OPEN_READ == state.mode;
}


void Os::Test::File::Tester::Read::action(
        Os::Test::File::Tester &state //!< The test state
) {
    U8 buffer[FILE_DATA_MAXIMUM];
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    FwSizeType size_desired = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM));
    FwSizeType size_read = size_desired;
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    Os::File::Status status = state.file.read(buffer, size_read, wait);
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
    return Os::File::Mode::OPEN_CREATE <= state.mode;
}


void Os::Test::File::Tester::Write::action(
        Os::Test::File::Tester &state //!< The test state
) {
    U8 buffer[FILE_DATA_MAXIMUM];
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FwSizeType size_desired = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM));
    FwSizeType size_written = size_desired;
    bool wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    for (FwSizeType i = 0; i < size_desired; i++) {
        buffer[i] = static_cast<U8>(STest::Pick::lowerUpper(0, std::numeric_limits<U8>::max()));
    }
    std::vector<U8> write_data(buffer, buffer + size_desired);
    Os::File::Status status = state.file.write(buffer, size_written, wait);
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
    return Os::File::Mode::OPEN_NO_MODE < state.mode;
}


void Os::Test::File::Tester::Seek::action(
        Os::Test::File::Tester &state //!< The test state
) {
    FwSizeType seek_offset = 0;
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();

    // Choose some random values
    bool absolute = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    if (absolute) {
        seek_offset = STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM);
    } else {
        seek_offset = STest::Pick::lowerUpper(0, original_file_state.position + FILE_DATA_MAXIMUM) - original_file_state.position;
    }
    Os::File::Status status = state.file.seek(seek_offset, absolute);
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
    return Os::File::Mode::OPEN_CREATE <= state.mode;
}


void Os::Test::File::Tester::Preallocate::action(
        Os::Test::File::Tester &state //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    FwSizeType offset = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM));
    FwSizeType length = static_cast<FwSizeType>(STest::Pick::lowerUpper(0, FILE_DATA_MAXIMUM));
    Os::File::Status status = state.file.preallocate(offset, length);
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
    return Os::File::Mode::OPEN_CREATE <= state.mode;
}


void Os::Test::File::Tester::Flush::action(
        Os::Test::File::Tester &state //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    Os::File::Status status = state.file.flush();
    ASSERT_EQ(Os::File::Status::OP_OK, status);
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
    return Os::File::Mode::OPEN_NO_MODE != state.mode;
}

void Os::Test::File::Tester::OpenInvalidModes::action(Os::Test::File::Tester &state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    // Check initial file state
    state.assert_file_opened(state.current_path);
    std::shared_ptr<const std::string> filename = state.get_filename(true);
    Os::File::Status status = state.file.open(filename->c_str(), Os::File::Mode::OPEN_CREATE);
    state.assert_valid_mode_status(status);
    state.assert_file_opened(state.current_path);  // Original file remains open
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
    return Os::File::Mode::OPEN_NO_MODE == state.mode;
}

void Os::Test::File::Tester::PreallocateWithoutOpen::action(Os::Test::File::Tester &state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    FwSizeType random_offset = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());
    FwSizeType random_size = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());

    Os::File::Status status = state.file.preallocate(random_offset, random_size);
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
    return Os::File::Mode::OPEN_NO_MODE == state.mode;
}

void Os::Test::File::Tester::SeekWithoutOpen::action(Os::Test::File::Tester &state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    // Check initial file state
    state.assert_file_closed();
    // Open file of given filename
    FwSizeType random_offset = STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max());
    bool random_absolute = static_cast<bool>(STest::Pick::lowerUpper(0, 1));

    Os::File::Status status = state.file.seek(random_offset, random_absolute);
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
    return Os::File::Mode::OPEN_NO_MODE < state.mode;;
}

void Os::Test::File::Tester::SeekInvalidSize::action(Os::Test::File::Tester &state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    // Open file of given filename
    FwSizeType random_offset = STest::Pick::lowerUpper(original_file_state.position + 1, std::numeric_limits<U32>::max());
    ASSERT_GT(random_offset, original_file_state.position);

    Os::File::Status status = state.file.seek(-1 * random_offset, false);
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
    return Os::File::Mode::OPEN_NO_MODE == state.mode || Os::File::Mode::OPEN_READ == state.mode;
}

void Os::Test::File::Tester::FlushInvalidModes::action(Os::Test::File::Tester &state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_TRUE(Os::File::Mode::OPEN_NO_MODE == state.file.m_mode || Os::File::Mode::OPEN_READ == state.file.m_mode);
    Os::File::Status status = state.file.flush();
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
    return Os::File::Mode::OPEN_READ != state.mode;
}

void Os::Test::File::Tester::ReadInvalidModes::action(Os::Test::File::Tester &state  //!< The test state
) {
    U8 buffer[10];
    FwSizeType size = sizeof buffer;
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_NE(Os::File::Mode::OPEN_READ, state.file.m_mode);
    Os::File::Status status = state.file.read(buffer, size, static_cast<bool>(STest::Pick::lowerUpper(0, 1)));
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
    return Os::File::Mode::OPEN_NO_MODE == state.mode || Os::File::Mode::OPEN_READ == state.mode;
}

void Os::Test::File::Tester::WriteInvalidModes::action(Os::Test::File::Tester &state  //!< The test state
) {
    U8 buffer[10];
    FwSizeType size = sizeof buffer;
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FileState original_file_state = state.current_file_state();
    ASSERT_TRUE(Os::File::Mode::OPEN_NO_MODE == state.file.m_mode || Os::File::Mode::OPEN_READ == state.file.m_mode);
    Os::File::Status status = state.file.write(buffer, size, static_cast<bool>(STest::Pick::lowerUpper(0, 1)));
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
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    Os::File::Mode random_mode =
            static_cast<Os::File::Mode>(STest::Pick::lowerUpper(Os::File::Mode::OPEN_READ,
                                                                Os::File::Mode::OPEN_APPEND));
    bool overwrite = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.open(nullptr, random_mode, overwrite), ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenIllegalMode
//
// ------------------------------------------------------------------------------------------------------

Os::Test::File::Tester::OpenIllegalMode::OpenIllegalMode() : Os::Test::File::Tester::AssertRule("OpenIllegalMode") {}

void Os::Test::File::Tester::OpenIllegalMode::action(Os::Test::File::Tester &state  //!< The test state
) {
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    std::shared_ptr<const std::string> random_filename = state.get_filename(true);
    bool overwrite = static_cast<bool>(STest::Pick::lowerUpper(0, 1));

    // Loop until we find a non-valid mode randomly
    U32 any = 0;
    do {
        any = STest::Pick::any();
    } while ((any != static_cast<U32>(Os::File::Mode::OPEN_NO_MODE)) &&
             (any < static_cast<U32>(Os::File::Mode::MAX_OPEN_MODE)));
    ASSERT_DEATH_IF_SUPPORTED(state.file.open(random_filename->c_str(), static_cast<Os::File::Mode>(any), overwrite),
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
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FwSizeType length = static_cast<FwSizeType>(STest::Pick::any());
    FwSizeType invalid_offset =
            -1 * static_cast<FwSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    ASSERT_DEATH_IF_SUPPORTED(state.file.preallocate(invalid_offset, length),
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
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FwSizeType offset = static_cast<FwSizeType>(STest::Pick::any());
    FwSizeType invalid_length =
            -1 * static_cast<FwSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    ASSERT_DEATH_IF_SUPPORTED(state.file.preallocate(offset, invalid_length),
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
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    ASSERT_DEATH_IF_SUPPORTED(state.file.seek(-1, true), Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
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
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FwSizeType size = static_cast<FwSizeType>(STest::Pick::any());
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.read(nullptr, size, random_wait), Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
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
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FwSizeType invalid_size = -1 * static_cast<FwSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.read(buffer, invalid_size, random_wait),
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
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FwSizeType size = static_cast<FwSizeType>(STest::Pick::any());
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.write(nullptr, size, random_wait), Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
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
    printf("--> Rule: %s \n", this->name);
    state.assert_file_consistent();
    FwSizeType invalid_size = -1 * static_cast<FwSizeType>(STest::Pick::lowerUpper(0, std::numeric_limits<U32>::max()));
    bool random_wait = static_cast<bool>(STest::Pick::lowerUpper(0, 1));
    ASSERT_DEATH_IF_SUPPORTED(state.file.read(buffer, invalid_size, random_wait),
                              Os::Test::File::Tester::ASSERT_IN_FILE_CPP);
    state.assert_file_consistent();
}
