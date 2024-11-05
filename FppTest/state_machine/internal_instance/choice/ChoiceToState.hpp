// ======================================================================
// \title  ChoiceToState.hpp
// \author bocchino
// \brief  hpp file for ChoiceToState component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceChoice_ChoiceToState_HPP
#define FppTest_SmInstanceChoice_ChoiceToState_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/choice/ChoiceToStateComponentAc.hpp"

namespace FppTest {

namespace SmInstanceChoice {

class ChoiceToState : public ChoiceToStateComponentBase {
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
        ENTER_S3,
    };

    //! The type FppTest_SmChoice_ChoiceToState
    using SmChoice_ChoiceToState = FppTest_SmChoice_ChoiceToState;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct ChoiceToState object
    ChoiceToState(const char* const compName  //!< The component name
    );

    //! Destroy ChoiceToState object
    ~ChoiceToState();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action exitS1 of state machine FppTest_SmChoice_ChoiceToState
    //!
    //! Exit S1
    void FppTest_SmChoice_ChoiceToState_action_exitS1(SmId smId,  //!< The state machine id
                                                      FppTest_SmChoice_ChoiceToState::Signal signal  //!< The signal
                                                      ) override;

    //! Implementation for action a of state machine FppTest_SmChoice_ChoiceToState
    //!
    //! Action a
    void FppTest_SmChoice_ChoiceToState_action_a(SmId smId,  //!< The state machine id
                                                 FppTest_SmChoice_ChoiceToState::Signal signal  //!< The signal
                                                 ) override;

    //! Implementation for action enterS2 of state machine FppTest_SmChoice_ChoiceToState
    //!
    //! Enter S2
    void FppTest_SmChoice_ChoiceToState_action_enterS2(SmId smId,  //!< The state machine id
                                                       FppTest_SmChoice_ChoiceToState::Signal signal  //!< The signal
                                                       ) override;

    //! Implementation for action enterS3 of state machine FppTest_SmChoice_ChoiceToState
    //!
    //! Enter S3
    void FppTest_SmChoice_ChoiceToState_action_enterS3(SmId smId,  //!< The state machine id
                                                       FppTest_SmChoice_ChoiceToState::Signal signal  //!< The signal
                                                       ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmChoice_ChoiceToState
    //!
    //! Guard g
    bool FppTest_SmChoice_ChoiceToState_guard_g(SmId smId,                                     //!< The state machine id
                                                FppTest_SmChoice_ChoiceToState::Signal signal  //!< The signal
    ) const override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the test with the true guard
    void testTrue();

    //! Run the test with the false guard
    void testFalse();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The action history for smChoiceChoiceToState
    SmHarness::SignalValueHistory<SmChoice_ChoiceToState::Signal, ActionId, historySize>
        m_smChoiceChoiceToState_actionHistory;

    //! The guard g for smChoiceChoiceToState
    SmHarness::NoArgGuard<SmChoice_ChoiceToState::Signal, historySize> m_smChoiceChoiceToState_guard_g;
};

}  // namespace SmInstanceChoice

}  // namespace FppTest

#endif
