// ======================================================================
// \title  StateToState.hpp
// \author bocchino
// \brief  hpp file for StateToState component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_StateToState_HPP
#define FppTest_SmInstanceState_StateToState_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToStateComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class StateToState : public StateToStateComponentBase {
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    //! The history size
    static constexpr FwSizeType historySize = 10;

    //! The queue depth
    static constexpr FwSizeType queueDepth = 10;

    //! The instance ID
    static constexpr FwEnumStoreType instanceId = 0;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct StateToState object
    StateToState(const char* const compName  //!< The component name
    );

    //! Destroy StateToState object
    ~StateToState();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_StateToState
    using SmState_StateToState = FppTest_SmState_StateToState;

    //! Action IDs
    enum class ActionId { EXIT_S1, EXIT_S2, EXIT_S3, A, ENTER_S1, ENTER_S2, ENTER_S3, ENTER_S4, ENTER_S5 };

  PRIVATE:
            // ----------------------------------------------------------------------
            // Implementations for internal state machine actions
            // ----------------------------------------------------------------------
    //! Implementation for action exitS1 of state machine FppTest_SmState_StateToState
    //!
    //! Exit S1
    void FppTest_SmState_StateToState_action_exitS1(SmId smId,  //!< The state machine id
                                                    FppTest_SmState_StateToState::Signal signal  //!< The signal
                                                    ) override;

    //! Implementation for action exitS2 of state machine FppTest_SmState_StateToState
    //!
    //! Exit S2
    void FppTest_SmState_StateToState_action_exitS2(SmId smId,  //!< The state machine id
                                                    FppTest_SmState_StateToState::Signal signal  //!< The signal
                                                    ) override;

    //! Implementation for action exitS3 of state machine FppTest_SmState_StateToState
    //!
    //! Exit S3
    void FppTest_SmState_StateToState_action_exitS3(SmId smId,  //!< The state machine id
                                                    FppTest_SmState_StateToState::Signal signal  //!< The signal
                                                    ) override;

    //! Implementation for action a of state machine FppTest_SmState_StateToState
    //!
    //! Action a
    void FppTest_SmState_StateToState_action_a(SmId smId,                                   //!< The state machine id
                                               FppTest_SmState_StateToState::Signal signal  //!< The signal
                                               ) override;

    //! Implementation for action enterS1 of state machine FppTest_SmState_StateToState
    //!
    //! Enter S1
    void FppTest_SmState_StateToState_action_enterS1(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToState::Signal signal  //!< The signal
                                                     ) override;

    //! Implementation for action enterS2 of state machine FppTest_SmState_StateToState
    //!
    //! Enter S2
    void FppTest_SmState_StateToState_action_enterS2(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToState::Signal signal  //!< The signal
                                                     ) override;

    //! Implementation for action enterS3 of state machine FppTest_SmState_StateToState
    //!
    //! Enter S3
    void FppTest_SmState_StateToState_action_enterS3(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToState::Signal signal  //!< The signal
                                                     ) override;

    //! Implementation for action enterS4 of state machine FppTest_SmState_StateToState
    //!
    //! Enter S4
    void FppTest_SmState_StateToState_action_enterS4(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToState::Signal signal  //!< The signal
                                                     ) override;

    //! Implementation for action enterS5 of state machine FppTest_SmState_StateToState
    //!
    //! Enter S5
    void FppTest_SmState_StateToState_action_enterS5(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToState::Signal signal  //!< The signal
                                                     ) override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test initial transition
    void testInit();

    //! Test transition S2 to S3
    void testS2_to_S3();

    //! Test transition S2 to S4
    void testS2_to_S4();

    //! Test transition S2 to S5
    void testS2_to_S5();

    //! Test transition S3 to S4
    void testS3_to_S4();

    //! Test transition S3 to S5
    void testS3_to_S5();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The action history for smStateStateToState
    SmHarness::SignalValueHistory<SmState_StateToState::Signal, ActionId, historySize>
        m_smStateStateToState_actionHistory;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
