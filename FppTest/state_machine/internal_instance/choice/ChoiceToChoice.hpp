// ======================================================================
// \title  ChoiceToChoice.hpp
// \author bocchino
// \brief  hpp file for ChoiceToChoice component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceChoice_ChoiceToChoice_HPP
#define FppTest_SmInstanceChoice_ChoiceToChoice_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/choice/ChoiceToChoiceComponentAc.hpp"

namespace FppTest {

namespace SmInstanceChoice {

class ChoiceToChoice : public ChoiceToChoiceComponentBase {
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

    //! Action IDs
    enum class ActionId {
        EXIT_S1,
        A,
        ENTER_S2,
    };

    //! The type FppTest_SmChoice_ChoiceToChoice
    using SmChoice_ChoiceToChoice = FppTest_SmChoice_ChoiceToChoice;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ChoiceToChoice object
    ChoiceToChoice(const char* const compName  //!< The component name
    );

    //! Destroy ChoiceToChoice object
    ~ChoiceToChoice();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action exitS1 of state machine FppTest_SmChoice_ChoiceToChoice
    //!
    //! Exit S1
    void FppTest_SmChoice_ChoiceToChoice_action_exitS1(SmId smId,  //!< The state machine id
                                                       FppTest_SmChoice_ChoiceToChoice::Signal signal  //!< The signal
                                                       ) override;

    //! Implementation for action a of state machine FppTest_SmChoice_ChoiceToChoice
    //!
    //! Action a
    void FppTest_SmChoice_ChoiceToChoice_action_a(SmId smId,  //!< The state machine id
                                                  FppTest_SmChoice_ChoiceToChoice::Signal signal  //!< The signal
                                                  ) override;

    //! Implementation for action enterS2 of state machine FppTest_SmChoice_ChoiceToChoice
    //!
    //! Enter S2
    void FppTest_SmChoice_ChoiceToChoice_action_enterS2(SmId smId,  //!< The state machine id
                                                        FppTest_SmChoice_ChoiceToChoice::Signal signal  //!< The signal
                                                        ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g1 of state machine FppTest_SmChoice_ChoiceToChoice
    //!
    //! Guard g1
    bool FppTest_SmChoice_ChoiceToChoice_guard_g1(SmId smId,  //!< The state machine id
                                                  FppTest_SmChoice_ChoiceToChoice::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard g2 of state machine FppTest_SmChoice_ChoiceToChoice
    //!
    //! Guard g2
    bool FppTest_SmChoice_ChoiceToChoice_guard_g2(SmId smId,  //!< The state machine id
                                                  FppTest_SmChoice_ChoiceToChoice::Signal signal  //!< The signal
    ) const override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Overflow hook implementations for internal state machines
    // ----------------------------------------------------------------------

    //! Overflow hook implementation for smChoiceChoiceToChoice
    void smChoiceChoiceToChoice_stateMachineOverflowHook(SmId smId,                       //!< The state machine ID
                                                         FwEnumStoreType signal,          //!< The signal
                                                         Fw::SerializeBufferBase& buffer  //!< The message buffer
                                                         ) override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the test with g1 true
    void testG1True();

    //! Run the test with g1 false and g2 true
    void testG1FalseG2True();

    //! Run the test with g1 false and g2 false
    void testG1FalseG2False();

    //! Test with queue overflow
    void testOverflow();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! Whether the overflow hook was called
    bool m_hookCalled = false;

    //! The action history for smChoiceChoiceToChoice
    SmHarness::SignalValueHistory<SmChoice_ChoiceToChoice::Signal, ActionId, historySize>
        m_smChoiceChoiceToChoice_actionHistory;

    //! The guard g1 for smChoiceChoiceToChoice
    SmHarness::NoArgGuard<SmChoice_ChoiceToChoice::Signal, historySize> m_smChoiceChoiceToChoice_guard_g1;

    //! The guard g2 for smChoiceChoiceToChoice
    SmHarness::NoArgGuard<SmChoice_ChoiceToChoice::Signal, historySize> m_smChoiceChoiceToChoice_guard_g2;
};

}  // namespace SmInstanceChoice

}  // namespace FppTest

#endif
