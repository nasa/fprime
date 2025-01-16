// ======================================================================
// \title  BasicTestArray.hpp
// \author bocchino
// \brief  hpp file for BasicTestArray component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicTestArray_HPP
#define FppTest_SmInstanceState_BasicTestArray_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicTestArrayComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicTestArray : public BasicTestArrayComponentBase {
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

    //! Construct BasicTestArray object
    BasicTestArray(const char* const compName  //!< The component name
    );

    //! Destroy BasicTestArray object
    ~BasicTestArray();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_BasicTestArray
    using SmState_BasicTestArray = FppTest_SmState_BasicTestArray;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicTestArray
    //!
    //! Action a
    void FppTest_SmState_BasicTestArray_action_a(SmId smId,  //!< The state machine id
                                                 FppTest_SmState_BasicTestArray::Signal signal  //!< The signal
                                                 ) override;

    //! Implementation for action b of state machine FppTest_SmState_BasicTestArray
    //!
    //! Action b
    void FppTest_SmState_BasicTestArray_action_b(SmId smId,  //!< The state machine id
                                                 FppTest_SmState_BasicTestArray::Signal signal,  //!< The signal
                                                 const SmHarness::TestArray& value               //!< The value
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

    //! The history associated with action a of smStateBasicTestArray
    SmHarness::History<SmState_BasicTestArray::Signal, historySize> m_smStateBasicTestArray_action_a_history;

    //! The history associated with action b of smStateBasicTestArray
    SmHarness::SignalValueHistory<SmState_BasicTestArray::Signal, SmHarness::TestArray, historySize>
        m_smStateBasicTestArray_action_b_history;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
