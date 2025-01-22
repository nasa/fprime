// ======================================================================
// \title  BasicTestEnum.hpp
// \author bocchino
// \brief  hpp file for BasicTestEnum component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicTestEnum_HPP
#define FppTest_SmInstanceState_BasicTestEnum_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestEnumComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicTestEnum : public BasicTestEnumComponentBase {
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

    //! Construct BasicTestEnum object
    BasicTestEnum(const char* const compName  //!< The component name
    );

    //! Destroy BasicTestEnum object
    ~BasicTestEnum();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_BasicTestEnum
    using SmState_BasicTestEnum = FppTest_SmState_BasicTestEnum;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicTestEnum
    //!
    //! Action a
    void FppTest_SmState_BasicTestEnum_action_a(SmId smId,                                    //!< The state machine id
                                                FppTest_SmState_BasicTestEnum::Signal signal  //!< The signal
                                                ) override;

    //! Implementation for action b of state machine FppTest_SmState_BasicTestEnum
    //!
    //! Action b
    void FppTest_SmState_BasicTestEnum_action_b(SmId smId,                                     //!< The state machine id
                                                FppTest_SmState_BasicTestEnum::Signal signal,  //!< The signal
                                                const SmHarness::TestEnum& value               //!< The value
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

    //! The history associated with action a of smStateBasicTestEnum
    SmHarness::History<SmState_BasicTestEnum::Signal, historySize> m_smStateBasicTestEnum_action_a_history;

    //! The history associated with action b of smStateBasicTestEnum
    SmHarness::SignalValueHistory<SmState_BasicTestEnum::Signal, SmHarness::TestEnum, historySize>
        m_smStateBasicTestEnum_action_b_history;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
