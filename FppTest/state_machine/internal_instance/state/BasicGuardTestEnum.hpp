// ======================================================================
// \title  BasicGuardTestEnum.hpp
// \author bocchino
// \brief  hpp file for BasicGuardTestEnum component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicGuardTestEnum_HPP
#define FppTest_SmInstanceState_BasicGuardTestEnum_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicGuardTestEnumComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicGuardTestEnum : public BasicGuardTestEnumComponentBase {
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

    //! The type FppTest_SmState_BasicGuardTestEnum
    using SmState_BasicGuardTestEnum = FppTest_SmState_BasicGuardTestEnum;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicGuardTestEnum object
    BasicGuardTestEnum(const char* const compName  //!< The component name
    );

    //! Destroy BasicGuardTestEnum object
    ~BasicGuardTestEnum();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicGuardTestEnum
    //!
    //! Action a
    void FppTest_SmState_BasicGuardTestEnum_action_a(SmId smId,  //!< The state machine id
                                                     FppTest_SmState_BasicGuardTestEnum::Signal signal,  //!< The signal
                                                     const SmHarness::TestEnum& value                    //!< The value
                                                     ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmState_BasicGuardTestEnum
    //!
    //! Guard g
    bool FppTest_SmState_BasicGuardTestEnum_guard_g(SmId smId,  //!< The state machine id
                                                    FppTest_SmState_BasicGuardTestEnum::Signal signal,  //!< The signal
                                                    const SmHarness::TestEnum& value                    //!< The value
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

    //! The history associated with action a of smStateBasicGuardTestEnum
    SmHarness::SignalValueHistory<SmState_BasicGuardTestEnum::Signal, SmHarness::TestEnum, historySize>
        m_smStateBasicGuardTestEnum_action_a_history;

    //! The guard g of smStateBasicGuardTestEnum
    SmHarness::Guard<SmState_BasicGuardTestEnum::Signal, SmHarness::TestEnum, historySize>
        m_smStateBasicGuardTestEnum_guard_g;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
