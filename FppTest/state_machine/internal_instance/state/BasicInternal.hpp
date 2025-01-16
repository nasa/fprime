// ======================================================================
// \title  BasicInternal.hpp
// \author bocchino
// \brief  hpp file for BasicInternal component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicInternal_HPP
#define FppTest_SmInstanceState_BasicInternal_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicInternalComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicInternal : public BasicInternalComponentBase {
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

    //! The type FppTest_SmState_BasicInternal
    using SmState_BasicInternal = FppTest_SmState_BasicInternal;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicInternal object
    BasicInternal(const char* const compName  //!< The component name
    );

    //! Destroy BasicInternal object
    ~BasicInternal();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicInternal
    //!
    //! Action a
    void FppTest_SmState_BasicInternal_action_a(SmId smId,                                    //!< The state machine id
                                                FppTest_SmState_BasicInternal::Signal signal  //!< The signal
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

    //! The history associated with action a of smStateBasicInternal
    SmHarness::History<SmState_BasicInternal::Signal, historySize> m_smStateBasicInternal_action_a_history;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
