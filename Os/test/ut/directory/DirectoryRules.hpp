// ======================================================================
// \title Os/test/ut/directory/DirectoryRules.hpp
// \brief rule definitions for common testing of directory
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  LockDirectory: Lock a directory successfully
// ------------------------------------------------------------------------------------------------------
struct LockDirectory : public STest::Rule<Os::Test::Directory::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    LockDirectory();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Directory::Tester &state  //!< The test state
    );

    //! Action
    void action(Os::Test::Directory::Tester &state  //!< The test state
    );
};
