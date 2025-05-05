// ======================================================================
// \title  BasicTestStruct.hpp
// \author bocchino
// \brief  hpp file for BasicTestStruct component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicTestStruct_HPP
#define FppTest_SmInstanceState_BasicTestStruct_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestStructComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicTestStruct : public BasicTestStructComponentBase {
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

    //! Construct BasicTestStruct object
    BasicTestStruct(const char* const compName  //!< The component name
    );

    //! Destroy BasicTestStruct object
    ~BasicTestStruct();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_BasicTestStruct
    using SmState_BasicTestStruct = FppTest_SmState_BasicTestStruct;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicTestStruct
    //!
    //! Action a
    void FppTest_SmState_BasicTestStruct_action_a(SmId smId,  //!< The state machine id
                                                  FppTest_SmState_BasicTestStruct::Signal signal  //!< The signal
                                                  ) override;

    //! Implementation for action b of state machine FppTest_SmState_BasicTestStruct
    //!
    //! Action b
    void FppTest_SmState_BasicTestStruct_action_b(SmId smId,  //!< The state machine id
                                                  FppTest_SmState_BasicTestStruct::Signal signal,  //!< The signal
                                                  const SmHarness::TestStruct& value               //!< The value
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

    //! The history associated with action a of smStateBasicTestStruct
    SmHarness::History<SmState_BasicTestStruct::Signal, historySize> m_smStateBasicTestStruct_action_a_history;

    //! The history associated with action b of smStateBasicTestStruct
    SmHarness::SignalValueHistory<SmState_BasicTestStruct::Signal, SmHarness::TestStruct, historySize>
        m_smStateBasicTestStruct_action_b_history;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
