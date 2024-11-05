// ======================================================================
// \title  BasicGuard.hpp
// \author bocchino
// \brief  hpp file for BasicGuard component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicGuard_HPP
#define FppTest_SmInstanceState_BasicGuard_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicGuard : public BasicGuardComponentBase {
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

    //! The type FppTest_SmState_BasicGuard
    using SmState_BasicGuard = FppTest_SmState_BasicGuard;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicGuard object
    BasicGuard(const char* const compName  //!< The component name
    );

    //! Destroy BasicGuard object
    ~BasicGuard();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicGuard
    //!
    //! Action a
    void FppTest_SmState_BasicGuard_action_a(SmId smId,                                 //!< The state machine id
                                             FppTest_SmState_BasicGuard::Signal signal  //!< The signal
                                             ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmState_BasicGuard
    //!
    //! Guard g
    bool FppTest_SmState_BasicGuard_guard_g(SmId smId,                                 //!< The state machine id
                                            FppTest_SmState_BasicGuard::Signal signal  //!< The signal
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
    void checkActionsAndGuards(FwSizeType expectedActionSize,  //!< The expected action size
                               FwSizeType expectedGuardSize    //!< The expected guard size
    );

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of smStateBasicGuard
    SmHarness::History<SmState_BasicGuard::Signal, historySize> m_smStateBasicGuard_action_a_history;

    //! The guard g of smStateBasicGuard
    SmHarness::NoArgGuard<SmState_BasicGuard::Signal, historySize> m_smStateBasicGuard_guard_g;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
