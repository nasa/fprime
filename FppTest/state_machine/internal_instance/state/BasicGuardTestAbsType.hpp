// ======================================================================
// \title  BasicGuardTestAbsType.hpp
// \author bocchino
// \brief  hpp file for BasicGuardTestAbsType component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicGuardTestAbsType_HPP
#define FppTest_SmInstanceState_BasicGuardTestAbsType_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestAbsTypeComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicGuardTestAbsType : public BasicGuardTestAbsTypeComponentBase {
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

    //! The type FppTest_SmState_BasicGuardTestAbsType
    using SmState_BasicGuardTestAbsType = FppTest_SmState_BasicGuardTestAbsType;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicGuardTestAbsType object
    BasicGuardTestAbsType(const char* const compName  //!< The component name
    );

    //! Destroy BasicGuardTestAbsType object
    ~BasicGuardTestAbsType();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicGuardTestAbsType
    //!
    //! Action a
    void FppTest_SmState_BasicGuardTestAbsType_action_a(
        SmId smId,                                             //!< The state machine id
        FppTest_SmState_BasicGuardTestAbsType::Signal signal,  //!< The signal
        const SmHarness::TestAbsType& value                    //!< The value
        ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmState_BasicGuardTestAbsType
    //!
    //! Guard g
    bool FppTest_SmState_BasicGuardTestAbsType_guard_g(
        SmId smId,                                             //!< The state machine id
        FppTest_SmState_BasicGuardTestAbsType::Signal signal,  //!< The signal
        const SmHarness::TestAbsType& value                    //!< The value
    ) const override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Overflow hook implementations for internal state machines
    // ----------------------------------------------------------------------

    //! Overflow hook implementation for smStateBasicGuardTestAbsType
    void smStateBasicGuardTestAbsType_stateMachineOverflowHook(SmId smId,               //!< The state machine ID
                                                               FwEnumStoreType signal,  //!< The signal
                                                               Fw::SerializeBufferBase& buffer  //!< The message buffer
                                                               ) override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test with true guard
    void testTrue();

    //! Test with false guard
    void testFalse();

    //! Test with queue overflow
    void testOverflow();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The test value
    SmHarness::TestAbsType m_value;

    //! Whether the overflow hook was called
    bool m_hookCalled = false;

    //! The history associated with action a of smStateBasicGuardTestAbsType
    SmHarness::SignalValueHistory<SmState_BasicGuardTestAbsType::Signal, SmHarness::TestAbsType, historySize>
        m_smStateBasicGuardTestAbsType_action_a_history;

    //! The guard g of smStateBasicGuardTestAbsType
    SmHarness::Guard<SmState_BasicGuardTestAbsType::Signal, SmHarness::TestAbsType, historySize>
        m_smStateBasicGuardTestAbsType_guard_g;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
