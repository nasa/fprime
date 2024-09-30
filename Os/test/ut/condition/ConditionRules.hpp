// ======================================================================
// \title Os/test/ut/condition/ConditionRules.hpp
// \brief condition variables rule definitions
// ======================================================================

// ------------------------------------------------------------------------------------------------------
// Rule:  Wait
//
// ------------------------------------------------------------------------------------------------------
struct Wait : public ConcurrentRule<Os::Test::Condition::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    explicit Wait(AggregatedConcurrentRule<Os::Test::Condition::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Condition::Tester& state  //!< The test state
                      ) override;

    //! Action
    void action(Os::Test::Condition::Tester& state  //!< The test state
                ) override;
};

// ------------------------------------------------------------------------------------------------------
// Rule:  Notify
//
// ------------------------------------------------------------------------------------------------------
struct Notify : public ConcurrentRule<Os::Test::Condition::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    explicit Notify(AggregatedConcurrentRule<Os::Test::Condition::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Condition::Tester& state  //!< The test state
                      ) override;

    //! Action
    void action(Os::Test::Condition::Tester& state  //!< The test state
                ) override;
};


// ------------------------------------------------------------------------------------------------------
// Rule:  NotifyAll
//
// ------------------------------------------------------------------------------------------------------
struct NotifyAll : public ConcurrentRule<Os::Test::Condition::Tester> {
    // ----------------------------------------------------------------------
    // Construction
    // ----------------------------------------------------------------------

    //! Constructor
    explicit NotifyAll(AggregatedConcurrentRule<Os::Test::Condition::Tester>& runner);

    // ----------------------------------------------------------------------
    // Public member functions
    // ----------------------------------------------------------------------

    //! Precondition
    bool precondition(const Os::Test::Condition::Tester& state  //!< The test state
                      ) override;

    //! Action
    void action(Os::Test::Condition::Tester& state  //!< The test state
                ) override;

};
