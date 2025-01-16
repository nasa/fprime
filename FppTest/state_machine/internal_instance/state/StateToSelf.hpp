// ======================================================================
// \title  StateToSelf.hpp
// \author bocchino
// \brief  hpp file for StateToSelf component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_StateToSelf_HPP
#define FppTest_SmInstanceState_StateToSelf_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToSelfComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class StateToSelf : public StateToSelfComponentBase {
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

    //! Construct StateToSelf object
    StateToSelf(const char* const compName  //!< The component name
    );

    //! Destroy StateToSelf object
    ~StateToSelf();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_StateToSelf
    using SmState_StateToSelf = FppTest_SmState_StateToSelf;

    //! Action IDs
    enum class ActionId { EXIT_S1, EXIT_S2, EXIT_S3, A, ENTER_S1, ENTER_S2, ENTER_S3 };

  PRIVATE:
            // ----------------------------------------------------------------------
            // Implementations for internal state machine actions
            // ----------------------------------------------------------------------
    //! Implementation for action exitS1 of state machine FppTest_SmState_StateToSelf
    //!
    //! Exit S1
    void FppTest_SmState_StateToSelf_action_exitS1(SmId smId,                                  //!< The state machine id
                                                   FppTest_SmState_StateToSelf::Signal signal  //!< The signal
                                                   ) override;

    //! Implementation for action exitS2 of state machine FppTest_SmState_StateToSelf
    //!
    //! Exit S2
    void FppTest_SmState_StateToSelf_action_exitS2(SmId smId,                                  //!< The state machine id
                                                   FppTest_SmState_StateToSelf::Signal signal  //!< The signal
                                                   ) override;

    //! Implementation for action exitS3 of state machine FppTest_SmState_StateToSelf
    //!
    //! Exit S3
    void FppTest_SmState_StateToSelf_action_exitS3(SmId smId,                                  //!< The state machine id
                                                   FppTest_SmState_StateToSelf::Signal signal  //!< The signal
                                                   ) override;

    //! Implementation for action a of state machine FppTest_SmState_StateToSelf
    //!
    //! Action a
    void FppTest_SmState_StateToSelf_action_a(SmId smId,                                  //!< The state machine id
                                              FppTest_SmState_StateToSelf::Signal signal  //!< The signal
                                              ) override;

    //! Implementation for action enterS1 of state machine FppTest_SmState_StateToSelf
    //!
    //! Enter S1
    void FppTest_SmState_StateToSelf_action_enterS1(SmId smId,  //!< The state machine id
                                                    FppTest_SmState_StateToSelf::Signal signal  //!< The signal
                                                    ) override;

    //! Implementation for action enterS2 of state machine FppTest_SmState_StateToSelf
    //!
    //! Enter S2
    void FppTest_SmState_StateToSelf_action_enterS2(SmId smId,  //!< The state machine id
                                                    FppTest_SmState_StateToSelf::Signal signal  //!< The signal
                                                    ) override;

    //! Implementation for action enterS3 of state machine FppTest_SmState_StateToSelf
    //!
    //! Enter S3
    void FppTest_SmState_StateToSelf_action_enterS3(SmId smId,  //!< The state machine id
                                                    FppTest_SmState_StateToSelf::Signal signal  //!< The signal
                                                    ) override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test initial transition
    void testInit();

    //! Test transition S2 to S1
    void testS2_to_S1();

    //! Test transition S2 to S3
    void testS2_to_S3();

    //! Test transition S3 to S1
    void testS3_to_S1();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The action history for smStateStateToSelf
    SmHarness::SignalValueHistory<SmState_StateToSelf::Signal, ActionId, historySize>
        m_smStateStateToSelf_actionHistory;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
