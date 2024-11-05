// ======================================================================
// \title  Sequence.hpp
// \author bocchino
// \brief  hpp file for Sequence component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceChoice_Sequence_HPP
#define FppTest_SmInstanceChoice_Sequence_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/choice/SequenceComponentAc.hpp"

namespace FppTest {

namespace SmInstanceChoice {

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

    //! The type FppTest_SmChoice_Sequence
    using SmChoice_Sequence = FppTest_SmChoice_Sequence;

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

    //! Implementation for action a of state machine FppTest_SmChoice_Sequence
    //!
    //! Action a
    void FppTest_SmChoice_Sequence_action_a(SmId smId,                                //!< The state machine id
                                            FppTest_SmChoice_Sequence::Signal signal  //!< The signal
                                            ) override;

    //! Implementation for action b of state machine FppTest_SmChoice_Sequence
    //!
    //! Action b
    void FppTest_SmChoice_Sequence_action_b(SmId smId,                                //!< The state machine id
                                            FppTest_SmChoice_Sequence::Signal signal  //!< The signal
                                            ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g1 of state machine FppTest_SmChoice_Sequence
    //!
    //! Guard g1
    bool FppTest_SmChoice_Sequence_guard_g1(SmId smId,                                //!< The state machine id
                                            FppTest_SmChoice_Sequence::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard g2 of state machine FppTest_SmChoice_Sequence
    //!
    //! Guard g2
    bool FppTest_SmChoice_Sequence_guard_g2(SmId smId,                                //!< The state machine id
                                            FppTest_SmChoice_Sequence::Signal signal  //!< The signal
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

    //! The history associated with action a of smChoiceSequence
    SmHarness::History<SmChoice_Sequence::Signal, historySize> m_smChoiceSequence_action_a_history;

    //! The history associated with action b of smChoiceSequence
    SmHarness::History<SmChoice_Sequence::Signal, historySize> m_smChoiceSequence_action_b_history;

    //! The guard g1 of smChoiceSequence
    SmHarness::NoArgGuard<SmChoice_Sequence::Signal, historySize> m_smChoiceSequence_guard_g1;

    //! The guard g2 of smChoiceSequence
    SmHarness::NoArgGuard<SmChoice_Sequence::Signal, historySize> m_smChoiceSequence_guard_g2;
};

}  // namespace SmInstanceChoice

}  // namespace FppTest

#endif
