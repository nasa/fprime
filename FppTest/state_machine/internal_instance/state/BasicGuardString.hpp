// ======================================================================
// \title  BasicGuardString.hpp
// \author bocchino
// \brief  hpp file for BasicGuardString component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicGuardString_HPP
#define FppTest_SmInstanceState_BasicGuardString_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardStringComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicGuardString : public BasicGuardStringComponentBase {
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

    //! The type FppTest_SmState_BasicGuardString
    using SmState_BasicGuardString = FppTest_SmState_BasicGuardString;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicGuardString object
    BasicGuardString(const char* const compName  //!< The component name
    );

    //! Destroy BasicGuardString object
    ~BasicGuardString();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicGuardString
    //!
    //! Action a
    void FppTest_SmState_BasicGuardString_action_a(SmId smId,  //!< The state machine id
                                                   FppTest_SmState_BasicGuardString::Signal signal,  //!< The signal
                                                   const Fw::StringBase& value                       //!< The value
                                                   ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmState_BasicGuardString
    //!
    //! Guard g
    bool FppTest_SmState_BasicGuardString_guard_g(SmId smId,  //!< The state machine id
                                                  FppTest_SmState_BasicGuardString::Signal signal,  //!< The signal
                                                  const Fw::StringBase& value                       //!< The value
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

    //! The history associated with action a of smStateBasicGuardString
    SmHarness::SignalValueHistory<SmState_BasicGuardString::Signal, Fw::String, historySize>
        m_smStateBasicGuardString_action_a_history;

    //! The guard g of smStateBasicGuardString
    SmHarness::Guard<SmState_BasicGuardString::Signal, Fw::String, historySize> m_smStateBasicGuardString_guard_g;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
