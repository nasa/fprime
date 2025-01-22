// ======================================================================
// \title  BasicGuardTestArray.hpp
// \author bocchino
// \brief  hpp file for BasicGuardTestArray component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicGuardTestArray_HPP
#define FppTest_SmInstanceState_BasicGuardTestArray_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestArrayComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicGuardTestArray : public BasicGuardTestArrayComponentBase {
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

    //! The type FppTest_SmState_BasicGuardTestArray
    using SmState_BasicGuardTestArray = FppTest_SmState_BasicGuardTestArray;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicGuardTestArray object
    BasicGuardTestArray(const char* const compName  //!< The component name
    );

    //! Destroy BasicGuardTestArray object
    ~BasicGuardTestArray();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicGuardTestArray
    //!
    //! Action a
    void FppTest_SmState_BasicGuardTestArray_action_a(
        SmId smId,                                           //!< The state machine id
        FppTest_SmState_BasicGuardTestArray::Signal signal,  //!< The signal
        const SmHarness::TestArray& value                    //!< The value
        ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmState_BasicGuardTestArray
    //!
    //! Guard g
    bool FppTest_SmState_BasicGuardTestArray_guard_g(
        SmId smId,                                           //!< The state machine id
        FppTest_SmState_BasicGuardTestArray::Signal signal,  //!< The signal
        const SmHarness::TestArray& value                    //!< The value
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
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of smStateBasicGuardTestArray
    SmHarness::SignalValueHistory<SmState_BasicGuardTestArray::Signal, SmHarness::TestArray, historySize>
        m_smStateBasicGuardTestArray_action_a_history;

    //! The guard g of smStateBasicGuardTestArray
    SmHarness::Guard<SmState_BasicGuardTestArray::Signal, SmHarness::TestArray, historySize>
        m_smStateBasicGuardTestArray_guard_g;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
