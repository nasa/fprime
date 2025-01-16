// ======================================================================
// \title  BasicTestAbsType.hpp
// \author bocchino
// \brief  hpp file for BasicTestAbsType component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicTestAbsType_HPP
#define FppTest_SmInstanceState_BasicTestAbsType_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestAbsTypeComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicTestAbsType : public BasicTestAbsTypeComponentBase {
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

    //! Construct BasicTestAbsType object
    BasicTestAbsType(const char* const compName  //!< The component name
    );

    //! Destroy BasicTestAbsType object
    ~BasicTestAbsType();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_BasicTestAbsType
    using SmState_BasicTestAbsType = FppTest_SmState_BasicTestAbsType;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicTestAbsType
    //!
    //! Action a
    void FppTest_SmState_BasicTestAbsType_action_a(SmId smId,  //!< The state machine id
                                                   FppTest_SmState_BasicTestAbsType::Signal signal  //!< The signal
                                                   ) override;

    //! Implementation for action b of state machine FppTest_SmState_BasicTestAbsType
    //!
    //! Action b
    void FppTest_SmState_BasicTestAbsType_action_b(SmId smId,  //!< The state machine id
                                                   FppTest_SmState_BasicTestAbsType::Signal signal,  //!< The signal
                                                   const SmHarness::TestAbsType& value               //!< The value
                                                   ) override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the test
    void test();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of smStateBasicTestAbsType
    SmHarness::History<SmState_BasicTestAbsType::Signal, historySize> m_smStateBasicTestAbsType_action_a_history;

    //! The history associated with action b of smStateBasicTestAbsType
    SmHarness::SignalValueHistory<SmState_BasicTestAbsType::Signal, SmHarness::TestAbsType, historySize>
        m_smStateBasicTestAbsType_action_b_history;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
