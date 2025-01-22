// ======================================================================
// \title  BasicU32.hpp
// \author bocchino
// \brief  hpp file for BasicU32 component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceChoice_BasicU32_HPP
#define FppTest_SmInstanceChoice_BasicU32_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/choice/BasicU32ComponentAc.hpp"

namespace FppTest {

namespace SmInstanceChoice {

class BasicU32 : public BasicU32ComponentBase {
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

    //! The type FppTest_SmChoice_BasicU32
    using SmChoice_BasicU32 = FppTest_SmChoice_BasicU32;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicU32 object
    BasicU32(const char* const compName  //!< The component name
    );

    //! Destroy BasicU32 object
    ~BasicU32();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmChoice_BasicU32
    //!
    //! Action a
    void FppTest_SmChoice_BasicU32_action_a(SmId smId,                                 //!< The state machine id
                                            FppTest_SmChoice_BasicU32::Signal signal,  //!< The signal
                                            U32 value                                  //!< The value
                                            ) override;

    //! Implementation for action b of state machine FppTest_SmChoice_BasicU32
    //!
    //! Action b
    void FppTest_SmChoice_BasicU32_action_b(SmId smId,                                //!< The state machine id
                                            FppTest_SmChoice_BasicU32::Signal signal  //!< The signal
                                            ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmChoice_BasicU32
    //!
    //! Guard g
    bool FppTest_SmChoice_BasicU32_guard_g(SmId smId,                                 //!< The state machine id
                                           FppTest_SmChoice_BasicU32::Signal signal,  //!< The signal
                                           U32 value                                  //!< The value
    ) const override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the test with the true guard
    void testTrue();

    //! Run the test with the false guard
    void testFalse();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of smChoiceBasicU32
    SmHarness::SignalValueHistory<SmChoice_BasicU32::Signal, U32, historySize> m_smChoiceBasicU32_action_a_history;

    //! The history associated with action b of smChoiceBasicU32
    SmHarness::History<SmChoice_BasicU32::Signal, historySize> m_smChoiceBasicU32_action_b_history;

    //! The guard g of smChoiceBasicU32
    SmHarness::Guard<SmChoice_BasicU32::Signal, U32, historySize> m_smChoiceBasicU32_guard_g;
};

}  // namespace SmInstanceChoice

}  // namespace FppTest

#endif
