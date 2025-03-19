// ======================================================================
// \title  StateToChild.hpp
// \author bocchino
// \brief  hpp file for StateToChild component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_StateToChild_HPP
#define FppTest_SmInstanceState_StateToChild_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToChildComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class StateToChild : public StateToChildComponentBase {
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

    //! Construct StateToChild object
    StateToChild(const char* const compName  //!< The component name
    );

    //! Destroy StateToChild object
    ~StateToChild();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_StateToChild
    using SmState_StateToChild = FppTest_SmState_StateToChild;

    //! Action IDs
    enum class ActionId { EXIT_S2, EXIT_S3, A, ENTER_S2, ENTER_S3 };

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action exitS2 of state machine FppTest_SmState_StateToChild
    //!
    //! Exit S2
    void FppTest_SmState_StateToChild_action_exitS2(SmId smId,  //!< The state machine id
                                                    FppTest_SmState_StateToChild::Signal signal  //!< The signal
                                                    ) override;

    //! Implementation for action exitS3 of state machine FppTest_SmState_StateToChild
    //!
    //! Exit S3
    void FppTest_SmState_StateToChild_action_exitS3(SmId smId,  //!< The state machine id
                                                    FppTest_SmState_StateToChild::Signal signal  //!< The signal
                                                    ) override;

    //! Implementation for action a of state machine FppTest_SmState_StateToChild
    //!
    //! Action a
    void FppTest_SmState_StateToChild_action_a(SmId smId,                                   //!< The state machine id
                                               FppTest_SmState_StateToChild::Signal signal  //!< The signal
                                               ) override;

    //! Implementation for action enterS2 of state machine FppTest_SmState_StateToChild
    //!
    //! Enter S2
    void FppTest_SmState_StateToChild_action_enterS2(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToChild::Signal signal  //!< The signal
                                                     ) override;

    //! Implementation for action enterS3 of state machine FppTest_SmState_StateToChild
    //!
    //! Enter S3
    void FppTest_SmState_StateToChild_action_enterS3(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToChild::Signal signal  //!< The signal
                                                     ) override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test initial transition
    void testInit();

    //! Test transition from S2 to S2
    void testS2_to_S2();

    //! Test transition from S2 to S3
    void testS2_to_S3();

    //! Test transition from S3 to S2
    void testS3_to_S2();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The action history for smStateStateToChild
    SmHarness::SignalValueHistory<SmState_StateToChild::Signal, ActionId, historySize>
        m_smStateStateToChild_actionHistory;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
