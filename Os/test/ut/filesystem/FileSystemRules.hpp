// ======================================================================
// \title Os/test/ut/filesystem/FileSystemRules.hpp
// \brief rule definitions for common testing of filesystem
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  LockFileSystem: Lock a filesystem successfully
// ------------------------------------------------------------------------------------------------------
struct LockFileSystem : public STest::Rule<Os::Test::FileSystem::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    LockFileSystem();

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
