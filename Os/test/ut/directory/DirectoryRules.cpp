// ======================================================================
// \title Os/test/ut/directory/DirectoryRules.cpp
// \brief rule implementations for common testing of directory
// ======================================================================

#include "RulesHeaders.hpp"
#include "DirectoryRules.hpp"
#include "STest/Pick/Pick.hpp"
#include <string>


// ------------------------------------------------------------------------------------------------------
// Rule:  Open -> Open a directory
// ------------------------------------------------------------------------------------------------------

Os::Test::Directory::Tester::Open::Open() :
    STest::Rule<Os::Test::Directory::Tester>("Open") {}

bool Os::Test::Directory::Tester::Open::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state != Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::Open::action(Os::Test::Directory::Tester &state) {
    Os::Directory::OpenMode mode = STest::Pick::lowerUpper(0, 1) == 1 ? Os::Directory::READ : Os::Directory::CREATE_IF_MISSING;
    Os::Directory::Status status = state.m_directory.open(state.m_path.c_str(), mode);
    ASSERT_EQ(status, Os::Directory::Status::OP_OK);
    state.m_state = Os::Test::Directory::Tester::DirectoryState::OPEN;
    state.m_seek_position = 0;
}

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenAlreadyExistsError -> Open an already existing directory with CREATE_EXCLUSIVE and error
// ------------------------------------------------------------------------------------------------------

Os::Test::Directory::Tester::OpenAlreadyExistsError::OpenAlreadyExistsError() :
    STest::Rule<Os::Test::Directory::Tester>("OpenAlreadyExistsError") {}

bool Os::Test::Directory::Tester::OpenAlreadyExistsError::precondition(const Os::Test::Directory::Tester &state) {
    return true;
}

void Os::Test::Directory::Tester::OpenAlreadyExistsError::action(Os::Test::Directory::Tester &state) {
    Os::Directory new_directory;
    Os::Directory::Status status = new_directory.open(state.m_path.c_str(), Os::Directory::OpenMode::CREATE_EXCLUSIVE);
    ASSERT_EQ(status, Os::Directory::Status::ALREADY_EXISTS);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Close -> Close a directory
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::Close::Close() :
    STest::Rule<Os::Test::Directory::Tester>("Close") {}

bool Os::Test::Directory::Tester::Close::precondition(const Os::Test::Directory::Tester &state) {
    return true;
}

void Os::Test::Directory::Tester::Close::action(Os::Test::Directory::Tester &state) {
    state.m_directory.close();
    ASSERT_FALSE(state.m_directory.isOpen());
    state.m_state = Os::Test::Directory::Tester::DirectoryState::CLOSED;
}

// ------------------------------------------------------------------------------------------------------
// Rule:  IsOpen -> Check if a directory is open
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::IsOpen::IsOpen() :
    STest::Rule<Os::Test::Directory::Tester>("IsOpen") {}

bool Os::Test::Directory::Tester::IsOpen::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state == Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::IsOpen::action(Os::Test::Directory::Tester &state) {
    ASSERT_TRUE(state.m_directory.isOpen());
}

// ------------------------------------------------------------------------------------------------------
// Rule:  IsNotOpen -> Check if a directory is not open
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::IsNotOpen::IsNotOpen() :
    STest::Rule<Os::Test::Directory::Tester>("IsNotOpen") {}

bool Os::Test::Directory::Tester::IsNotOpen::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state != Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::IsNotOpen::action(Os::Test::Directory::Tester &state) {
    ASSERT_FALSE(state.m_directory.isOpen());
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadOneFile -> Read one file from a directory and assert name
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::ReadOneFile::ReadOneFile() :
    STest::Rule<Os::Test::Directory::Tester>("ReadOneFile") {}

bool Os::Test::Directory::Tester::ReadOneFile::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state == Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::ReadOneFile::action(Os::Test::Directory::Tester &state) {
    ASSERT_TRUE(state.m_directory.isOpen());
    char filename[100];
    Os::Directory::Status status = state.m_directory.read(filename, 100);
    // If seek is at the end of the directory, expect NO_MORE_FILES - otherwise expect normal read and valid filename
    if (state.m_seek_position < static_cast<FwIndexType>(state.m_filenames.size())) {
        ASSERT_EQ(status, Os::Directory::Status::OP_OK);
        ASSERT_TRUE(state.is_valid_filename(std::string(filename)));
        state.m_seek_position++;
    } else {
        ASSERT_EQ(status, Os::Directory::Status::NO_MORE_FILES);
    }
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadOneFileString -> Read one file from a directory and assert name
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::ReadOneFileString::ReadOneFileString() :
    STest::Rule<Os::Test::Directory::Tester>("ReadOneFileString") {}

bool Os::Test::Directory::Tester::ReadOneFileString::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state == Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::ReadOneFileString::action(Os::Test::Directory::Tester &state) {
    ASSERT_TRUE(state.m_directory.isOpen());
    Fw::String filename;
    Os::Directory::Status status = state.m_directory.read(filename);
    // If seek is at the end of the directory, expect NO_MORE_FILES - otherwise expect normal read and valid filename
    if (state.m_seek_position < static_cast<FwIndexType>(state.m_filenames.size())) {
        ASSERT_EQ(status, Os::Directory::Status::OP_OK);
        ASSERT_TRUE(state.is_valid_filename(std::string(filename.toChar())));
        state.m_seek_position++;
    } else {
        ASSERT_EQ(status, Os::Directory::Status::NO_MORE_FILES);
    }
}

// ------------------------------------------------------------------------------------------------------
// Rule:  Rewind -> Rewind a directory
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::Rewind::Rewind() :
    STest::Rule<Os::Test::Directory::Tester>("Rewind") {}

bool Os::Test::Directory::Tester::Rewind::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state == Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::Rewind::action(Os::Test::Directory::Tester &state) {
    ASSERT_TRUE(state.m_directory.isOpen());
    Os::Directory::Status status = state.m_directory.rewind();
    ASSERT_EQ(status, Os::Directory::Status::OP_OK);
    state.m_seek_position = 0;
}

// ------------------------------------------------------------------------------------------------------
// Rule:  GetFileCount -> Get the file count in a directory
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::GetFileCount::GetFileCount() :
    STest::Rule<Os::Test::Directory::Tester>("GetFileCount") {}

bool Os::Test::Directory::Tester::GetFileCount::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state == Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::GetFileCount::action(Os::Test::Directory::Tester &state) {
    ASSERT_TRUE(state.m_directory.isOpen());
    FwSizeType fileCount;
    Os::Directory::Status status = state.m_directory.getFileCount(fileCount);
    ASSERT_EQ(status, Os::Directory::Status::OP_OK);
    ASSERT_EQ(fileCount, state.m_filenames.size());

    // NOTE: getFileCount reads in order to count
    // therefore it plays with seek_position. Should it be called out in interface? Or should Posix implementation change?
    state.m_seek_position = 0; // Reset seek position when getFileCount is called
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadAllFiles -> Read all files in a directory
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::ReadAllFiles::ReadAllFiles() :
    STest::Rule<Os::Test::Directory::Tester>("ReadAllFiles") {}

bool Os::Test::Directory::Tester::ReadAllFiles::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state == Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::ReadAllFiles::action(Os::Test::Directory::Tester &state) {
    ASSERT_TRUE(state.m_directory.isOpen());
    const FwSizeType arraySize = FW_MAX(state.m_filenames.size(), 1); // .size() can be 0 during testing so ensure at least 1
    Fw::String outArray[arraySize];
    FwSizeType outFileCount = 0;
    Os::Directory::Status status = state.m_directory.readDirectory(outArray, arraySize, outFileCount);
    ASSERT_EQ(status, Os::Directory::Status::OP_OK);
    // Number of files read should be the number of files in the directory minus the original seek position
    ASSERT_EQ(outFileCount, state.m_filenames.size());
    for (FwSizeType i = 0; i < outFileCount; i++) {
        ASSERT_TRUE(state.is_valid_filename(std::string(outArray[i].toChar())));
    }
    // readDirectory resets the seek position to the end
    state.m_seek_position = 0;
}

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadWithoutOpen -> Read a closed directory and expect Status::NOT_OPENED
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::ReadWithoutOpen::ReadWithoutOpen() :
    STest::Rule<Os::Test::Directory::Tester>("ReadWithoutOpen") {}

bool Os::Test::Directory::Tester::ReadWithoutOpen::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state != Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::ReadWithoutOpen::action(Os::Test::Directory::Tester &state) {
    ASSERT_FALSE(state.m_directory.isOpen());
    char unused[4];
    ASSERT_EQ(state.m_directory.read(unused, 4), Os::Directory::Status::NOT_OPENED);
}

// ------------------------------------------------------------------------------------------------------
// Rule:  RewindWithoutOpen -> Rewind a closed directory and expect Status::NOT_OPENED
// ------------------------------------------------------------------------------------------------------
Os::Test::Directory::Tester::RewindWithoutOpen::RewindWithoutOpen() :
    STest::Rule<Os::Test::Directory::Tester>("RewindWithoutOpen") {}

bool Os::Test::Directory::Tester::RewindWithoutOpen::precondition(const Os::Test::Directory::Tester &state) {
    return state.m_state != Os::Test::Directory::Tester::DirectoryState::OPEN;
}

void Os::Test::Directory::Tester::RewindWithoutOpen::action(Os::Test::Directory::Tester &state) {
    ASSERT_FALSE(state.m_directory.isOpen());
    ASSERT_EQ(state.m_directory.rewind(), Os::Directory::Status::NOT_OPENED);
}
