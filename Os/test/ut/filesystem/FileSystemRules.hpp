// ======================================================================
// \title Os/test/ut/filesystem/FileSystemRules.hpp
// \brief rule definitions for common testing of filesystem
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  DirectoryExists: Check that exists() returns true for an existing directory
// ------------------------------------------------------------------------------------------------------
struct DirectoryExists : public STest::Rule<Os::Test::FileSystem::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    DirectoryExists();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileSystem::Tester &state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileSystem::Tester &state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  FileExists: Check that exists() returns false for an existing file
// ------------------------------------------------------------------------------------------------------
struct FileExists : public STest::Rule<Os::Test::FileSystem::Tester> {
    FileExists();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  PathNotExists: Check that exists() returns false for a non-existing path
// ------------------------------------------------------------------------------------------------------
struct PathNotExists : public STest::Rule<Os::Test::FileSystem::Tester> {
    PathNotExists();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  RemoveFile: Check that removeFile() removes a file
// ------------------------------------------------------------------------------------------------------
struct RemoveFile : public STest::Rule<Os::Test::FileSystem::Tester> {
    RemoveFile();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  TouchFile: Check that touch() creates a file
// ------------------------------------------------------------------------------------------------------
struct TouchFile : public STest::Rule<Os::Test::FileSystem::Tester> {
    TouchFile();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  RemoveDirectory: Check directory removal
// ------------------------------------------------------------------------------------------------------
struct RemoveDirectory : public STest::Rule<Os::Test::FileSystem::Tester> {
    RemoveDirectory();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  CreateDirectory: Check directory removal
// ------------------------------------------------------------------------------------------------------
struct CreateDirectory : public STest::Rule<Os::Test::FileSystem::Tester> {
    CreateDirectory();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  MoveFile: Move a file from one location to another
// ------------------------------------------------------------------------------------------------------
struct MoveFile : public STest::Rule<Os::Test::FileSystem::Tester> {
    MoveFile();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  CopyFile: Move a file from one location to another
// ------------------------------------------------------------------------------------------------------
struct CopyFile : public STest::Rule<Os::Test::FileSystem::Tester> {
    CopyFile();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  AppendFile: Move a file from one location to another
// ------------------------------------------------------------------------------------------------------
struct AppendFile : public STest::Rule<Os::Test::FileSystem::Tester> {
    AppendFile();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  GetFileSize: Move a file from one location to another
// ------------------------------------------------------------------------------------------------------
struct GetFileSize : public STest::Rule<Os::Test::FileSystem::Tester> {
    GetFileSize();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  GetFreeSpace: Move a file from one location to another
// ------------------------------------------------------------------------------------------------------
struct GetFreeSpace : public STest::Rule<Os::Test::FileSystem::Tester> {
    GetFreeSpace();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ChangeWorkingDirectory: Move a file from one location to another
// ------------------------------------------------------------------------------------------------------
struct ChangeWorkingDirectory : public STest::Rule<Os::Test::FileSystem::Tester> {
    ChangeWorkingDirectory();
    bool precondition(const Os::Test::FileSystem::Tester &state);
    void action(Os::Test::FileSystem::Tester &state);
};
