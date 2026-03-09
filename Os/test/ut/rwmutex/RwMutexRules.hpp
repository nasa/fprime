// ======================================================================
// \title Os/test/ut/rwmutex/RwMutexRules.hpp
// \brief rule definitions for common testing of reader-writer mutex
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule:  LockRwMutex: Acquire exclusive (write) lock via lock()
// ------------------------------------------------------------------------------------------------------
struct LockRwMutex : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    LockRwMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  UnlockRwMutex - Release exclusive (write) lock via unlock()
// ------------------------------------------------------------------------------------------------------
struct UnlockRwMutex : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    UnlockRwMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  TakeRwMutex - Acquire exclusive (write) lock via take()
// ------------------------------------------------------------------------------------------------------
struct TakeRwMutex : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    TakeRwMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ReleaseRwMutex - Release exclusive (write) lock via release()
// ------------------------------------------------------------------------------------------------------
struct ReleaseRwMutex : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReleaseRwMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  LockReadRwMutex: Acquire shared (read) lock via lockRead()
// ------------------------------------------------------------------------------------------------------
struct LockReadRwMutex : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    LockReadRwMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  UnlockReadRwMutex - Release shared (read) lock via unlockRead()
// ------------------------------------------------------------------------------------------------------
struct UnlockReadRwMutex : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    UnlockReadRwMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  TakeReadRwMutex - Acquire shared (read) lock via takeRead()
// ------------------------------------------------------------------------------------------------------
struct TakeReadRwMutex : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    TakeReadRwMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ReleaseReadRwMutex - Release shared (read) lock via releaseRead()
// ------------------------------------------------------------------------------------------------------
struct ReleaseReadRwMutex : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReleaseReadRwMutex();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ProtectDataWrite - Write to shared data inside exclusive lock
// ------------------------------------------------------------------------------------------------------
struct ProtectDataWrite : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ProtectDataWrite();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ProtectDataRead - Read shared data inside shared lock
// ------------------------------------------------------------------------------------------------------
struct ProtectDataRead : public STest::Rule<Os::Test::RwMutex::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ProtectDataRead();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RwMutex::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::RwMutex::Tester& state  //!< The test state
    );
};
