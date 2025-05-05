// ======================================================================
// \title  BasicGuardU32.hpp
// \author bocchino
// \brief  hpp file for BasicGuardU32 component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicGuardU32_HPP
#define FppTest_SmInstanceState_BasicGuardU32_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardU32ComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicGuardU32 : public BasicGuardU32ComponentBase {
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

    //! The type FppTest_SmState_BasicGuardU32
    using SmState_BasicGuardU32 = FppTest_SmState_BasicGuardU32;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicGuardU32 object
    BasicGuardU32(const char* const compName  //!< The component name
    );

    //! Destroy BasicGuardU32 object
    ~BasicGuardU32();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicGuardU32
    //!
    //! Action a
    void FppTest_SmState_BasicGuardU32_action_a(SmId smId,                                     //!< The state machine id
                                                FppTest_SmState_BasicGuardU32::Signal signal,  //!< The signal
                                                U32 value                                      //!< The value
                                                ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmState_BasicGuardU32
    //!
    //! Guard g
    bool FppTest_SmState_BasicGuardU32_guard_g(SmId smId,                                     //!< The state machine id
                                               FppTest_SmState_BasicGuardU32::Signal signal,  //!< The signal
                                               U32 value                                      //!< The value
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

    //! The history associated with action a of smStateBasicGuardU32
    SmHarness::SignalValueHistory<SmState_BasicGuardU32::Signal, U32, historySize>
        m_smStateBasicGuardU32_action_a_history;

    //! The guard g of smStateBasicGuardU32
    SmHarness::Guard<SmState_BasicGuardU32::Signal, U32, historySize> m_smStateBasicGuardU32_guard_g;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
