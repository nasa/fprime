// ======================================================================
// \title  Basic.hpp
// \author bocchino
// \brief  hpp file for Basic component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceChoice_Basic_HPP
#define FppTest_SmInstanceChoice_Basic_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/choice/BasicComponentAc.hpp"

namespace FppTest {

namespace SmInstanceChoice {

class Basic : public BasicComponentBase {
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

    //! The type FppTest_SmInstanceChoice_Basic_Basic
    using Basic_Basic = FppTest_SmInstanceChoice_Basic_Basic;

    //! The type FppTest_SmChoice_Basic
    using SmChoice_Basic = FppTest_SmChoice_Basic;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct Basic object
    Basic(const char* const compName  //!< The component name
    );

    //! Destroy Basic object
    ~Basic();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Handler implementations for typed input ports
    // ----------------------------------------------------------------------

    //! Handler implementation for schedIn
    void schedIn_handler(FwIndexType portNum,  //!< The port number
                         U32 context           //!< The call order
                         ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmInstanceChoice_Basic_Basic
    //!
    //! Action a
    void FppTest_SmInstanceChoice_Basic_Basic_action_a(
        SmId smId,                                           //!< The state machine id
        FppTest_SmInstanceChoice_Basic_Basic::Signal signal  //!< The signal
        ) override;

    //! Implementation for action b of state machine FppTest_SmInstanceChoice_Basic_Basic
    //!
    //! Action b
    void FppTest_SmInstanceChoice_Basic_Basic_action_b(
        SmId smId,                                           //!< The state machine id
        FppTest_SmInstanceChoice_Basic_Basic::Signal signal  //!< The signal
        ) override;

    //! Implementation for action a of state machine FppTest_SmChoice_Basic
    //!
    //! Action a
    void FppTest_SmChoice_Basic_action_a(SmId smId,                             //!< The state machine id
                                         FppTest_SmChoice_Basic::Signal signal  //!< The signal
                                         ) override;

    //! Implementation for action b of state machine FppTest_SmChoice_Basic
    //!
    //! Action b
    void FppTest_SmChoice_Basic_action_b(SmId smId,                             //!< The state machine id
                                         FppTest_SmChoice_Basic::Signal signal  //!< The signal
                                         ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmInstanceChoice_Basic_Basic
    //!
    //! Guard g
    bool FppTest_SmInstanceChoice_Basic_Basic_guard_g(
        SmId smId,                                           //!< The state machine id
        FppTest_SmInstanceChoice_Basic_Basic::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard g of state machine FppTest_SmChoice_Basic
    //!
    //! Guard g
    bool FppTest_SmChoice_Basic_guard_g(SmId smId,                             //!< The state machine id
                                        FppTest_SmChoice_Basic::Signal signal  //!< The signal
    ) const override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the basic test with the true guard
    void testBasicTrue();

    //! Run the basic test with the false guard
    void testBasicFalse();

    //! Run the smChoiceBasic test with the true guard
    void testSmChoiceBasicTrue();

    //! Run the smChoiceSmChoiceBasic test with the false guard
    void testSmChoiceBasicFalse();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of basic
    SmHarness::History<Basic_Basic::Signal, historySize> m_basic_action_a_history;

    //! The history associated with action b of basic
    SmHarness::History<Basic_Basic::Signal, historySize> m_basic_action_b_history;

    //! The guard g of basic
    SmHarness::NoArgGuard<Basic_Basic::Signal, historySize> m_basic_guard_g;

    //! The history associated with action a of smChoiceBasic
    SmHarness::History<SmChoice_Basic::Signal, historySize> m_smChoiceBasic_action_a_history;

    //! The history associated with action b of smChoiceBasic
    SmHarness::History<SmChoice_Basic::Signal, historySize> m_smChoiceBasic_action_b_history;

    //! The guard g of smChoiceBasic
    SmHarness::NoArgGuard<SmChoice_Basic::Signal, historySize> m_smChoiceBasic_guard_g;
};

}  // namespace SmInstanceChoice

}  // namespace FppTest

#endif
