// ======================================================================
// \title Os/test/ut/rawtime/RawTimeRules.hpp
// \brief rule definitions for common testing of RawTime
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
// Rule SelfDiffIsZero: RawTime diff with itself is zero
// ------------------------------------------------------------------------------------------------------
struct SelfDiffIsZero : public STest::Rule<Os::Test::RawTime::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    SelfDiffIsZero();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::RawTime::Tester &state  //!< The test state
    );

    //! Action
    void action(Os::Test::RawTime::Tester &state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule Now: Get current RawTime
// ------------------------------------------------------------------------------------------------------
struct Now : public STest::Rule<Os::Test::RawTime::Tester> {
    Now();
    bool precondition(const Os::Test::RawTime::Tester &state);
    void action(Os::Test::RawTime::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule GetTimeDiffU32: Get diff between two RawTime
// ------------------------------------------------------------------------------------------------------
struct GetTimeDiffU32 : public STest::Rule<Os::Test::RawTime::Tester> {
    GetTimeDiffU32();
    bool precondition(const Os::Test::RawTime::Tester &state);
    void action(Os::Test::RawTime::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule GetTimeInterval: Get diff between two RawTime
// ------------------------------------------------------------------------------------------------------
struct GetTimeInterval : public STest::Rule<Os::Test::RawTime::Tester> {
    GetTimeInterval();
    bool precondition(const Os::Test::RawTime::Tester &state);
    void action(Os::Test::RawTime::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule Serialization: Serialize and Deserialize RawTime
// ------------------------------------------------------------------------------------------------------
struct Serialization : public STest::Rule<Os::Test::RawTime::Tester> {
    Serialization();
    bool precondition(const Os::Test::RawTime::Tester &state);
    void action(Os::Test::RawTime::Tester &state);
};

// ------------------------------------------------------------------------------------------------------
// Rule DiffU32Overflow: DiffU32 overflows if times are too far apart
// ------------------------------------------------------------------------------------------------------
struct DiffU32Overflow : public STest::Rule<Os::Test::RawTime::Tester> {
    DiffU32Overflow();
    bool precondition(const Os::Test::RawTime::Tester &state);
    void action(Os::Test::RawTime::Tester &state);
};
