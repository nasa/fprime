// ======================================================================
// \title  Internal.hpp
// \author bocchino
// \brief  hpp file for Internal component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_Internal_HPP
#define FppTest_SmInstanceState_Internal_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/InternalComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class Internal : public InternalComponentBase {
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

    //! Construct Internal object
    Internal(const char* const compName  //!< The component name
    );

    //! Destroy Internal object
    ~Internal();

  private:
    // ----------------------------------------------------------------------
    // Types
    // ----------------------------------------------------------------------

    //! The type FppTest_SmState_Internal
    using SmState_Internal = FppTest_SmState_Internal;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_Internal
    //!
    //! Action a
    void FppTest_SmState_Internal_action_a(SmId smId,                               //!< The state machine id
                                           FppTest_SmState_Internal::Signal signal  //!< The signal
                                           ) override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Test initial transition
    void testInit();

    //! Test internal transition in S2
    void testS2_internal();

    //! Test transition S2 to S3
    void testS2_to_S3();

    //! Test internal transition in S3
    void testS3_internal();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of smStateInternal
    SmHarness::History<SmState_Internal::Signal, historySize> m_smStateInternal_action_a_history;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
