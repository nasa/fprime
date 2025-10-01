// ======================================================================
// \title Os/test/ut/file/MyRules.hpp
// \brief rule definitions for common testing
// ======================================================================
// Stripped when compiled, here for IDEs
#include "RulesHeaders.hpp"

// ------------------------------------------------------------------------------------------------------
//  OpenFile: base rule for all open rules
//
// ------------------------------------------------------------------------------------------------------
struct OpenBaseRule : public STest::Rule<Os::Test::FileTest::Tester> {
    //! Constructor
    OpenBaseRule(const char* rule_name,
                 Os::File::Mode mode = Os::File::Mode::OPEN_CREATE,
                 const bool overwrite = false,
                 const bool randomize_filename = false);

    Os::File::Mode m_mode;
    Os::File::OverwriteType m_overwrite;
    bool m_random;

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenFileCreate
//
// ------------------------------------------------------------------------------------------------------
struct OpenFileCreate : public OpenBaseRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    explicit OpenFileCreate(const bool randomize_filename = false);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenFileCreateOverwrite
//
// ------------------------------------------------------------------------------------------------------
struct OpenFileCreateOverwrite : public OpenBaseRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    explicit OpenFileCreateOverwrite(const bool randomize_filename = false);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForAppend
//
// ------------------------------------------------------------------------------------------------------
struct OpenForAppend : public OpenBaseRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    explicit OpenForAppend(const bool randomize_filename = false);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForWrite
//
// ------------------------------------------------------------------------------------------------------
struct OpenForWrite : public OpenBaseRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    explicit OpenForWrite(const bool randomize_filename = false);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenForRead
//
// ------------------------------------------------------------------------------------------------------
struct OpenForRead : public OpenBaseRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    explicit OpenForRead(const bool randomize_filename = false);
};

// ------------------------------------------------------------------------------------------------------
// Rule:  CloseFile
//
// ------------------------------------------------------------------------------------------------------
struct CloseFile : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    CloseFile();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Read
//
// ------------------------------------------------------------------------------------------------------
struct Read : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Read();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Write
//
// ------------------------------------------------------------------------------------------------------
struct Write : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Write();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Seek
//
// ------------------------------------------------------------------------------------------------------
struct Seek : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Seek();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Preallocate
//
// ------------------------------------------------------------------------------------------------------
struct Preallocate : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Preallocate();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Flush
//
// ------------------------------------------------------------------------------------------------------
struct Flush : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Flush();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenInvalidModes
//
// ------------------------------------------------------------------------------------------------------
struct OpenInvalidModes : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    OpenInvalidModes();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  PreallocateWithoutOpen
//
// ------------------------------------------------------------------------------------------------------
struct PreallocateWithoutOpen : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    PreallocateWithoutOpen();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekWithoutOpen
//
// ------------------------------------------------------------------------------------------------------
struct SeekWithoutOpen : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    SeekWithoutOpen();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekInvalidSize
//
// ------------------------------------------------------------------------------------------------------
struct SeekInvalidSize : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    SeekInvalidSize();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  FlushInvalidModes
//
// ------------------------------------------------------------------------------------------------------
struct FlushInvalidModes : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    FlushInvalidModes();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadInvalidModes
//
// ------------------------------------------------------------------------------------------------------
struct ReadInvalidModes : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReadInvalidModes();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  WriteInvalidModes
//
// ------------------------------------------------------------------------------------------------------
struct WriteInvalidModes : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    WriteInvalidModes();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Base Rule:  AssertRule
//
// ------------------------------------------------------------------------------------------------------
struct AssertRule : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    explicit AssertRule(const char* name);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    virtual void action(Os::Test::FileTest::Tester& state  //!< The test state
                        ) = 0;
};

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenIllegalPath
//
// ------------------------------------------------------------------------------------------------------
struct OpenIllegalPath : public AssertRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    OpenIllegalPath();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
                ) override;
};

// ------------------------------------------------------------------------------------------------------
// Rule:  OpenIllegalMode
//
// ------------------------------------------------------------------------------------------------------
struct OpenIllegalMode : public AssertRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    OpenIllegalMode();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  SeekIllegal
//
// ------------------------------------------------------------------------------------------------------
struct SeekIllegal : public AssertRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    SeekIllegal();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ReadIllegalBuffer
//
// ------------------------------------------------------------------------------------------------------
struct ReadIllegalBuffer : public AssertRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReadIllegalBuffer();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  WriteIllegalBuffer
//
// ------------------------------------------------------------------------------------------------------
struct WriteIllegalBuffer : public AssertRule {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    WriteIllegalBuffer();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  CopyAssignment
//
// ------------------------------------------------------------------------------------------------------
struct CopyAssignment : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    CopyAssignment();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  CopyConstruction
//
// ------------------------------------------------------------------------------------------------------
struct CopyConstruction : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    CopyConstruction();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  FullCrc
//
// ------------------------------------------------------------------------------------------------------
struct FullCrc : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    FullCrc();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  IncrementalCrc
//
// ------------------------------------------------------------------------------------------------------
struct IncrementalCrc : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    IncrementalCrc();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  FinalizeCrc
//
// ------------------------------------------------------------------------------------------------------
struct FinalizeCrc : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    FinalizeCrc();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  FullCrcInvalidModes
//
// ------------------------------------------------------------------------------------------------------
struct FullCrcInvalidModes : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    FullCrcInvalidModes();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  IncrementalCrcInvalidModes
//
// ------------------------------------------------------------------------------------------------------
struct IncrementalCrcInvalidModes : public STest::Rule<Os::Test::FileTest::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    IncrementalCrcInvalidModes();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::FileTest::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::FileTest::Tester& state  //!< The test state
    );
};
