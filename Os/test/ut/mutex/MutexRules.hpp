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
// Rule:  LockBusyMutex: Lock a mutex that is already locked
// ------------------------------------------------------------------------------------------------------
struct LockBusyMutex : public STest::Rule<Os::Test::Mutex::Tester> {

    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    LockBusyMutex();

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
// Rule:  UnlockFreeMutex: Unlock a mutex that is not locked
// ------------------------------------------------------------------------------------------------------
struct UnlockFreeMutex : public STest::Rule<Os::Test::Mutex::Tester> {

    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    UnlockFreeMutex();

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

