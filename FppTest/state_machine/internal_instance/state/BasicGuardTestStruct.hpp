// ======================================================================
// \title  BasicGuardTestStruct.hpp
// \author bocchino
// \brief  hpp file for BasicGuardTestStruct component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicGuardTestStruct_HPP
#define FppTest_SmInstanceState_BasicGuardTestStruct_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestStructComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicGuardTestStruct : public BasicGuardTestStructComponentBase {
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

    //! The type FppTest_SmState_BasicGuardTestStruct
    using SmState_BasicGuardTestStruct = FppTest_SmState_BasicGuardTestStruct;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicGuardTestStruct object
    BasicGuardTestStruct(const char* const compName  //!< The component name
    );

    //! Destroy BasicGuardTestStruct object
    ~BasicGuardTestStruct();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicGuardTestStruct
    //!
    //! Action a
    void FppTest_SmState_BasicGuardTestStruct_action_a(
        SmId smId,                                            //!< The state machine id
        FppTest_SmState_BasicGuardTestStruct::Signal signal,  //!< The signal
        const SmHarness::TestStruct& value                    //!< The value
        ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmState_BasicGuardTestStruct
    //!
    //! Guard g
    bool FppTest_SmState_BasicGuardTestStruct_guard_g(
        SmId smId,                                            //!< The state machine id
        FppTest_SmState_BasicGuardTestStruct::Signal signal,  //!< The signal
        const SmHarness::TestStruct& value                    //!< The value
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

    //! The history associated with action a of smStateBasicGuardTestStruct
    SmHarness::SignalValueHistory<SmState_BasicGuardTestStruct::Signal, SmHarness::TestStruct, historySize>
        m_smStateBasicGuardTestStruct_action_a_history;

    //! The guard g of smStateBasicGuardTestStruct
    SmHarness::Guard<SmState_BasicGuardTestStruct::Signal, SmHarness::TestStruct, historySize>
        m_smStateBasicGuardTestStruct_guard_g;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
