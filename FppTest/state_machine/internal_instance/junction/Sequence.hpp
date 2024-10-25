// ======================================================================
// \title  Sequence.hpp
// \author bocchino
// \brief  hpp file for Sequence component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceJunction_Sequence_HPP
#define FppTest_SmInstanceJunction_Sequence_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/junction/SequenceComponentAc.hpp"

namespace FppTest {

namespace SmInstanceJunction {

class Sequence : public SequenceComponentBase {
  private:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! The history size
    static constexpr FwSizeType historySize = 10;

    //! The queue depth
    static constexpr FwSizeType queueDepth = 10;

    //! The instance ID
    static constexpr FwEnumStoreType instanceId = 0;

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmJunction_Sequence
    using SmJunction_Sequence = FppTest_SmJunction_Sequence;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Sequence object
    Sequence(const char* const compName  //!< The component name
    );

    //! Destroy Sequence object
    ~Sequence();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmJunction_Sequence
    //!
    //! Action a
    void FppTest_SmJunction_Sequence_action_a(SmId smId,                                  //!< The state machine id
                                              FppTest_SmJunction_Sequence::Signal signal  //!< The signal
                                              ) override;

    //! Implementation for action b of state machine FppTest_SmJunction_Sequence
    //!
    //! Action b
    void FppTest_SmJunction_Sequence_action_b(SmId smId,                                  //!< The state machine id
                                              FppTest_SmJunction_Sequence::Signal signal  //!< The signal
                                              ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g1 of state machine FppTest_SmJunction_Sequence
    //!
    //! Guard g1
    bool FppTest_SmJunction_Sequence_guard_g1(SmId smId,                                  //!< The state machine id
                                              FppTest_SmJunction_Sequence::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard g2 of state machine FppTest_SmJunction_Sequence
    //!
    //! Guard g2
    bool FppTest_SmJunction_Sequence_guard_g2(SmId smId,                                  //!< The state machine id
                                              FppTest_SmJunction_Sequence::Signal signal  //!< The signal
    ) const override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the test with g1 true
    void testG1True();

    //! Run the test with g1 true and g2 true
    void testG1FalseG2True();

    //! Run the test with g1 true and g2 false
    void testG1FalseG2False();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of smJunctionSequence
    SmHarness::History<SmJunction_Sequence::Signal, historySize> m_smJunctionSequence_action_a_history;

    //! The history associated with action b of smJunctionSequence
    SmHarness::History<SmJunction_Sequence::Signal, historySize> m_smJunctionSequence_action_b_history;

    //! The guard g1 of smJunctionSequence
    SmHarness::NoArgGuard<SmJunction_Sequence::Signal, historySize> m_smJunctionSequence_guard_g1;

    //! The guard g2 of smJunctionSequence
    SmHarness::NoArgGuard<SmJunction_Sequence::Signal, historySize> m_smJunctionSequence_guard_g2;
};

}  // namespace SmInstanceJunction

}  // namespace FppTest

#endif
