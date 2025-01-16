// ======================================================================
// \title  StateToChoice.hpp
// \author bocchino
// \brief  hpp file for StateToChoice component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_StateToChoice_HPP
#define FppTest_SmInstanceState_StateToChoice_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/StateToChoiceComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class StateToChoice : public StateToChoiceComponentBase {
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

    //! Construct StateToChoice object
    StateToChoice(const char* const compName  //!< The component name
    );

    //! Destroy StateToChoice object
    ~StateToChoice();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_StateToChoice
    using SmState_StateToChoice = FppTest_SmState_StateToChoice;

    //! Action IDs
    enum class ActionId { EXIT_S1, EXIT_S2, EXIT_S3, A, ENTER_S1, ENTER_S2, ENTER_S3, ENTER_S4 };

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action exitS1 of state machine FppTest_SmState_StateToChoice
    //!
    //! Exit S1
    void FppTest_SmState_StateToChoice_action_exitS1(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToChoice::Signal signal  //!< The signal
                                                     ) override;

    //! Implementation for action exitS2 of state machine FppTest_SmState_StateToChoice
    //!
    //! Exit S2
    void FppTest_SmState_StateToChoice_action_exitS2(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToChoice::Signal signal  //!< The signal
                                                     ) override;

    //! Implementation for action exitS3 of state machine FppTest_SmState_StateToChoice
    //!
    //! Exit S3
    void FppTest_SmState_StateToChoice_action_exitS3(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_StateToChoice::Signal signal  //!< The signal
                                                     ) override;

    //! Implementation for action a of state machine FppTest_SmState_StateToChoice
    //!
    //! Action a
    void FppTest_SmState_StateToChoice_action_a(SmId smId,                                    //!< The state machine id
                                                FppTest_SmState_StateToChoice::Signal signal  //!< The signal
                                                ) override;

    //! Implementation for action enterS1 of state machine FppTest_SmState_StateToChoice
    //!
    //! Enter S1
    void FppTest_SmState_StateToChoice_action_enterS1(SmId smId,  //!< The state machine id
                                                      FppTest_SmState_StateToChoice::Signal signal  //!< The signal
                                                      ) override;

    //! Implementation for action enterS2 of state machine FppTest_SmState_StateToChoice
    //!
    //! Enter S2
    void FppTest_SmState_StateToChoice_action_enterS2(SmId smId,  //!< The state machine id
                                                      FppTest_SmState_StateToChoice::Signal signal  //!< The signal
                                                      ) override;

    //! Implementation for action enterS3 of state machine FppTest_SmState_StateToChoice
    //!
    //! Enter S3
    void FppTest_SmState_StateToChoice_action_enterS3(SmId smId,  //!< The state machine id
                                                      FppTest_SmState_StateToChoice::Signal signal  //!< The signal
                                                      ) override;

    //! Implementation for action enterS4 of state machine FppTest_SmState_StateToChoice
    //!
    //! Enter S4
    void FppTest_SmState_StateToChoice_action_enterS4(SmId smId,  //!< The state machine id
                                                      FppTest_SmState_StateToChoice::Signal signal  //!< The signal
                                                      ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmState_StateToChoice
    //!
    //! Guard g
    bool FppTest_SmState_StateToChoice_guard_g(SmId smId,                                    //!< The state machine id
                                               FppTest_SmState_StateToChoice::Signal signal  //!< The signal
    ) const override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test initial transition
    void testInit();

    //! Test transition S2 to C
    void testS2_to_C();

    //! Test transition S2 to S3
    void testS2_to_S3();

    //! Test transition S2 to S4
    void testS2_to_S4();

    //! Test transition S3 to C
    void testS3_to_C();

    //! Test transition S3 to S4
    void testS3_to_S4();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The action history for SmStateStateToChoice
    SmHarness::SignalValueHistory<SmState_StateToChoice::Signal, ActionId, historySize>
        m_smStateStateToChoice_actionHistory;

    //! The guard g for SmStateStateToChoice
    SmHarness::NoArgGuard<SmState_StateToChoice::Signal, historySize> m_smStateStateToChoice_guard_g;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
