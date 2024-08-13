// ======================================================================
// \title Os/test/ut/mutex/MutexRules.hpp
// \brief rule definitions for common testing of mutex
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  LockMutex: Lock a mutex successfully
// ------------------------------------------------------------------------------------------------------
struct LockMutex : public STest::Rule<Os::Test::Mutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    LockMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Mutex::Tester &state  //!< The test state
    );

    //! Action
    void action(Os::Test::Mutex::Tester &state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  UnlockMutex: Unlock a mutex that is locked successfully
// ------------------------------------------------------------------------------------------------------
struct UnlockMutex : public STest::Rule<Os::Test::Mutex::Tester> {

    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    UnlockMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(
            const Os::Test::Mutex::Tester &state //!< The test state
    );

    //! Action
    void action(
            Os::Test::Mutex::Tester &state //!< The test state
    );

};


// ------------------------------------------------------------------------------------------------------
// Rule:  TakeMutex: Take a mutex successfully
// ------------------------------------------------------------------------------------------------------
struct TakeMutex : public STest::Rule<Os::Test::Mutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    TakeMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Mutex::Tester &state  //!< The test state
    );

    //! Action
    void action(Os::Test::Mutex::Tester &state  //!< The test state
    );
};


// ------------------------------------------------------------------------------------------------------
// Rule:  ReleaseMutex: Take a mutex successfully
// ------------------------------------------------------------------------------------------------------
struct ReleaseMutex : public STest::Rule<Os::Test::Mutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReleaseMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Mutex::Tester &state  //!< The test state
    );

    //! Action
    void action(Os::Test::Mutex::Tester &state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ProtectDataCheck: Check that data is protected by a mutex (running another task in parallel)
// ------------------------------------------------------------------------------------------------------
struct ProtectDataCheck : public STest::Rule<Os::Test::Mutex::Tester> {

    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ProtectDataCheck();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(
            const Os::Test::Mutex::Tester &state //!< The test state
    );

    //! Action
    void action(
            Os::Test::Mutex::Tester &state //!< The test state
    );

};
