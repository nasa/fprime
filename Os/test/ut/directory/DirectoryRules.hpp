// ======================================================================
// \title Os/test/ut/directory/DirectoryRules.hpp
// \brief rule definitions for common testing of directory
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule: Open a directory
// ------------------------------------------------------------------------------------------------------
struct OpenDirectory : public STest::Rule<Os::Test::Directory::Tester> {
    //! Constructor
    OpenDirectory();

    //! Precondition
    bool precondition(const Os::Test::Directory::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::Directory::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule: Close a directory
// ------------------------------------------------------------------------------------------------------
struct CloseDirectory : public STest::Rule<Os::Test::Directory::Tester> {
    CloseDirectory();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Open/Close directory and check isOpen return value
// Note: maybe not needed if we assert
// ------------------------------------------------------------------------------------------------------
struct IsOpenChange : public STest::Rule<Os::Test::Directory::Tester> {
    IsOpenChange();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};


// ------------------------------------------------------------------------------------------------------
// Rule: Read a directory
// ------------------------------------------------------------------------------------------------------
struct ReadDirectory : public STest::Rule<Os::Test::Directory::Tester> {
    ReadDirectory();
    bool precondition(const Os::Test::Directory::Tester& state);
    void action(Os::Test::Directory::Tester& state);
};

// ------------------------------------------------------------------------------------------------------
// Rule: Rewind a directory
// ------------------------------------------------------------------------------------------------------
struct RewindDirectory : public STest::Rule<Os::Test::Directory::Tester> {
    RewindDirectory();
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
// Rule: Read a closed directory and expect an error, with read inode call
// ------------------------------------------------------------------------------------------------------
struct ReadInodeWithoutOpen : public STest::Rule<Os::Test::Directory::Tester> {
    ReadInodeWithoutOpen();
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
