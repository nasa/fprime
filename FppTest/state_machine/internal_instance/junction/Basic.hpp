// ======================================================================
// \title  Basic.hpp
// \author bocchino
// \brief  hpp file for Basic component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceJunction_Basic_HPP
#define FppTest_SmInstanceJunction_Basic_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/junction/BasicComponentAc.hpp"

namespace FppTest {

namespace SmInstanceJunction {

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

    //! The type FppTest_SmInstanceJunction_Basic_Basic
    using Basic_Basic = FppTest_SmInstanceJunction_Basic_Basic;

    //! The type FppTest_SmJunction_Basic
    using SmJunction_Basic = FppTest_SmJunction_Basic;

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

    //! Implementation for action a of state machine FppTest_SmInstanceJunction_Basic_Basic
    //!
    //! Action a
    void FppTest_SmInstanceJunction_Basic_Basic_action_a(
        SmId smId,                                             //!< The state machine id
        FppTest_SmInstanceJunction_Basic_Basic::Signal signal  //!< The signal
        ) override;

    //! Implementation for action b of state machine FppTest_SmInstanceJunction_Basic_Basic
    //!
    //! Action b
    void FppTest_SmInstanceJunction_Basic_Basic_action_b(
        SmId smId,                                             //!< The state machine id
        FppTest_SmInstanceJunction_Basic_Basic::Signal signal  //!< The signal
        ) override;

    //! Implementation for action a of state machine FppTest_SmJunction_Basic
    //!
    //! Action a
    void FppTest_SmJunction_Basic_action_a(SmId smId,                               //!< The state machine id
                                           FppTest_SmJunction_Basic::Signal signal  //!< The signal
                                           ) override;

    //! Implementation for action b of state machine FppTest_SmJunction_Basic
    //!
    //! Action b
    void FppTest_SmJunction_Basic_action_b(SmId smId,                               //!< The state machine id
                                           FppTest_SmJunction_Basic::Signal signal  //!< The signal
                                           ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmInstanceJunction_Basic_Basic
    //!
    //! Guard g
    bool FppTest_SmInstanceJunction_Basic_Basic_guard_g(
        SmId smId,                                             //!< The state machine id
        FppTest_SmInstanceJunction_Basic_Basic::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard g of state machine FppTest_SmJunction_Basic
    //!
    //! Guard g
    bool FppTest_SmJunction_Basic_guard_g(SmId smId,                               //!< The state machine id
                                          FppTest_SmJunction_Basic::Signal signal  //!< The signal
    ) const override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the basic test with the true guard
    void testBasicTrue();

    //! Run the basic test with the false guard
    void testBasicFalse();

    //! Run the smJunctionBasic test with the true guard
    void testSmJunctionBasicTrue();

    //! Run the smJunctionSmJunctionBasic test with the false guard
    void testSmJunctionBasicFalse();

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

    //! The history associated with action a of smJunctionBasic
    SmHarness::History<SmJunction_Basic::Signal, historySize> m_smJunctionBasic_action_a_history;

    //! The history associated with action b of smJunctionBasic
    SmHarness::History<SmJunction_Basic::Signal, historySize> m_smJunctionBasic_action_b_history;

    //! The guard g of smJunctionBasic
    SmHarness::NoArgGuard<SmJunction_Basic::Signal, historySize> m_smJunctionBasic_guard_g;
};

}  // namespace SmInstanceJunction

}  // namespace FppTest

#endif
