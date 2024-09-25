// ======================================================================
// \title Os/test/ut/queue/QueueRules.hpp
// \brief queue rule definitions
// ======================================================================

// ------------------------------------------------------------------------------------------------------
// Rule:  Create
//
// ------------------------------------------------------------------------------------------------------
struct Create : public STest::Rule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Create();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  SendNotFull
//
// ------------------------------------------------------------------------------------------------------
struct SendNotFull : public STest::Rule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    SendNotFull();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  SendFullNoBlock
//
// ------------------------------------------------------------------------------------------------------
struct SendFullNoBlock : public STest::Rule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    SendFullNoBlock();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ReceiveNotEmpty
//
// ------------------------------------------------------------------------------------------------------
struct ReceiveNotEmpty : public STest::Rule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReceiveNotEmpty();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ReceiveEmptyNoBlock
//
// ------------------------------------------------------------------------------------------------------
struct ReceiveEmptyNoBlock : public STest::Rule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReceiveEmptyNoBlock();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Overflow
//
// ------------------------------------------------------------------------------------------------------
struct Overflow : public STest::Rule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Overflow();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Underflow
//
// ------------------------------------------------------------------------------------------------------
struct Underflow : public STest::Rule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    Underflow();

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
    );

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
    );
};

// ------------------------------------------------------------------------------------------------------
// Rule:  SendBlock
//
// ------------------------------------------------------------------------------------------------------
struct SendBlock : public ConcurrentRule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    SendBlock(AggregatedConcurrentRule<Os::Test::Queue::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
                      ) override;

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
                ) override;
};
// ------------------------------------------------------------------------------------------------------
// Rule:  SendUnblock
//
// ------------------------------------------------------------------------------------------------------
struct SendUnblock : public ConcurrentRule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    SendUnblock(AggregatedConcurrentRule<Os::Test::Queue::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
                      ) override;

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
                ) override;
};

// ------------------------------------------------------------------------------------------------------
// Rule:  ReceiveBlock
//
// ------------------------------------------------------------------------------------------------------
struct ReceiveBlock : public ConcurrentRule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReceiveBlock(AggregatedConcurrentRule<Os::Test::Queue::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
                      ) override;

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
                ) override;
};
// ------------------------------------------------------------------------------------------------------
// Rule:  ReceiveUnblock
//
// ------------------------------------------------------------------------------------------------------
struct ReceiveUnblock : public ConcurrentRule<Os::Test::Queue::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    ReceiveUnblock(AggregatedConcurrentRule<Os::Test::Queue::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Queue::Tester& state  //!< The test state
                      ) override;

    //! Action
    void action(Os::Test::Queue::Tester& state  //!< The test state
                ) override;
};