// ======================================================================
// \title  Nested.hpp
// \author bocchino
// \brief  hpp file for Nested component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceInitial_Nested_HPP
#define FppTest_SmInstanceInitial_Nested_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/initial/NestedComponentAc.hpp"

namespace FppTest {

namespace SmInstanceInitial {

class Nested : public NestedComponentBase {
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

    //! The type FppTest_SmInstanceInitial_Nested_Nested
    using Nested_Nested = FppTest_SmInstanceInitial_Nested_Nested;

    //! The type FppTest_SmInitial_Nested
    using SmInitial_Nested = FppTest_SmInitial_Nested;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Nested object
    Nested(const char* const compName  //!< The component name
    );

    //! Destroy Nested object
    ~Nested();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmInitial_Nested
    //!
    //! Action a
    void FppTest_SmInitial_Nested_action_a(SmId smId,                               //!< The state machine id
                                           FppTest_SmInitial_Nested::Signal signal  //!< The signal
                                           ) override;

    //! Implementation for action a of state machine FppTest_SmInstanceInitial_Nested_Nested
    //!
    //! Action a
    void FppTest_SmInstanceInitial_Nested_Nested_action_a(
        SmId smId,                                              //!< The state machine id
        FppTest_SmInstanceInitial_Nested_Nested::Signal signal  //!< The signal
        ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Overflow hook implementations for internal state machines
    // ----------------------------------------------------------------------

    //! Overflow hook implementation for smInitialNested
    void smInitialNested_stateMachineOverflowHook(SmId smId,                       //!< The state machine ID
                                                  FwEnumStoreType signal,          //!< The signal
                                                  Fw::SerializeBufferBase& buffer  //!< The message buffer
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

    //! The history associated with action a of nested
    SmHarness::History<Nested_Nested::Signal, historySize> m_nested_action_a_history;

    //! The history associated with action a of smInitialNested
    SmHarness::History<SmInitial_Nested::Signal, historySize> m_smInitialNested_action_a_history;
};

}  // namespace SmInstanceInitial

}  // namespace FppTest

#endif
