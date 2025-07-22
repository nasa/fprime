// ======================================================================
// \title  ChoiceTester.hpp
// \author bocchino
// \brief  hpp file for ChoiceTester component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceInitial_Choice_HPP
#define FppTest_SmInstanceInitial_Choice_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/initial/ChoiceComponentAc.hpp"

namespace FppTest {

namespace SmInstanceInitial {

class ChoiceTester : public ChoiceComponentBase {
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

    //! The type FppTest_SmInstanceInitial_Choice_Choice
    using Choice_Choice = FppTest_SmInstanceInitial_Choice_Choice;

    //! The type FppTest_SmInitial_Choice
    using SmInitial_Choice = FppTest_SmInitial_Choice;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ChoiceTester object
    ChoiceTester(const char* const compName  //!< The component name
    );

    //! Destroy ChoiceTester object
    ~ChoiceTester();

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmInitial_Choice
    //!
    //! Action a
    void FppTest_SmInitial_Choice_action_a(SmId smId,                               //!< The state machine id
                                           FppTest_SmInitial_Choice::Signal signal  //!< The signal
                                           ) override;

    //! Implementation for action a of state machine FppTest_SmInstanceInitial_Choice_Choice
    //!
    //! Action a
    void FppTest_SmInstanceInitial_Choice_Choice_action_a(
        SmId smId,                                              //!< The state machine id
        FppTest_SmInstanceInitial_Choice_Choice::Signal signal  //!< The signal
        ) override;

  private:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmInitial_Choice
    //!
    //! Guard g
    bool FppTest_SmInitial_Choice_guard_g(SmId smId,                               //!< The state machine id
                                          FppTest_SmInitial_Choice::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard g of state machine FppTest_SmInstanceInitial_Choice_Choice
    //!
    //! Guard g
    bool FppTest_SmInstanceInitial_Choice_Choice_guard_g(
        SmId smId,                                              //!< The state machine id
        FppTest_SmInstanceInitial_Choice_Choice::Signal signal  //!< The signal
    ) const override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test with true guard
    void testTrue();

    //! Test with false guard
    void testFalse();

  private:
    // ----------------------------------------------------------------------
    // Helper functions
    // ----------------------------------------------------------------------

    //! Helper function for checking actions and guards
    void checkActionsAndGuards(FwIndexType expectedActionSize,  //!< The expected action size
                               FwIndexType expectedGuardSize    //!< The expected guard size
    );

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of choice
    SmHarness::History<Choice_Choice::Signal, historySize> m_choice_action_a_history;

    //! The history associated with action a of smInitialChoice
    SmHarness::History<SmInitial_Choice::Signal, historySize> m_smInitialChoice_action_a_history;

    //! The guard g of choice
    SmHarness::NoArgGuard<Choice_Choice::Signal, historySize> m_choice_guard_g;

    //! The guard g of smInitialChoice
    SmHarness::NoArgGuard<SmInitial_Choice::Signal, historySize> m_smInitialChoice_guard_g;
};

}  // namespace SmInstanceInitial

}  // namespace FppTest

#endif
