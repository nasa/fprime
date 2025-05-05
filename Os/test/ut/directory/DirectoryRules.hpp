// ======================================================================
// \title Os/test/ut/directory/DirectoryRules.hpp
// \brief rule definitions for common testing of directory
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule: Open a directory
// ------------------------------------------------------------------------------------------------------
struct Open : public STest::Rule<Os::Test::Directory::Tester> {
    //! Constructor
    Open();

    //! Precondition
    bool precondition(const Os::Test::Directory::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::Directory::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule: OpenAlreadyExistsError
// ------------------------------------------------------------------------------------------------------
struct OpenAlreadyExistsError : public STest::Rule<Os::Test::Directory::Tester> {
    OpenAlreadyExistsError();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Close a directory
// ------------------------------------------------------------------------------------------------------
struct Close : public STest::Rule<Os::Test::Directory::Tester> {
    Close();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Check isOpen return true on an open directory
// Note: maybe not needed if we assert
// ------------------------------------------------------------------------------------------------------
struct IsOpen : public STest::Rule<Os::Test::Directory::Tester> {
    IsOpen();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Check isOpen return false on non-open directory
// Note: maybe not needed if we assert
// ------------------------------------------------------------------------------------------------------
struct IsNotOpen : public STest::Rule<Os::Test::Directory::Tester> {
    IsNotOpen();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Read the first file in a directory that has been opened
// ------------------------------------------------------------------------------------------------------
struct ReadOneFile : public STest::Rule<Os::Test::Directory::Tester> {
    ReadOneFile();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Read the first file in a directory that has been opened, using read(StringBase&) implementation
// ------------------------------------------------------------------------------------------------------
struct ReadOneFileString : public STest::Rule<Os::Test::Directory::Tester> {
    ReadOneFileString();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Rewind a directory
// ------------------------------------------------------------------------------------------------------
struct Rewind : public STest::Rule<Os::Test::Directory::Tester> {
    Rewind();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: GetFileCount of a directory
// ------------------------------------------------------------------------------------------------------
struct GetFileCount : public STest::Rule<Os::Test::Directory::Tester> {
    GetFileCount();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: ReadAllFiles from a directory
// ------------------------------------------------------------------------------------------------------
struct ReadAllFiles : public STest::Rule<Os::Test::Directory::Tester> {
    ReadAllFiles();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Read a closed directory and expect an error
// ------------------------------------------------------------------------------------------------------
struct ReadWithoutOpen : public STest::Rule<Os::Test::Directory::Tester> {
    ReadWithoutOpen();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Rewind a closed directory and expect an error
// ------------------------------------------------------------------------------------------------------
struct RewindWithoutOpen : public STest::Rule<Os::Test::Directory::Tester> {
    RewindWithoutOpen();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};
