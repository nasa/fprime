// ======================================================================
// \title  Basic.hpp
// \author bocchino
// \brief  hpp file for Basic component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceInitial_Basic_HPP
#define FppTest_SmInstanceInitial_Basic_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/initial/BasicComponentAc.hpp"

namespace FppTest {

namespace SmInstanceInitial {

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

    //! The type FppTest_SmInstanceInitial_Basic_Basic
    using Basic_Basic = FppTest_SmInstanceInitial_Basic_Basic;

    //! The type FppTest_SmInitial_Basic
    using SmInitial_Basic = FppTest_SmInitial_Basic;

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

    //! Implementation for action a of state machine FppTest_SmInitial_Basic
    //!
    //! Action a
    void FppTest_SmInitial_Basic_action_a(SmId smId,                              //!< The state machine id
                                          FppTest_SmInitial_Basic::Signal signal  //!< The signal
                                          ) override;

    //! Implementation for action a of state machine FppTest_SmInstanceInitial_Basic_Basic
    //!
    //! Action a
    void FppTest_SmInstanceInitial_Basic_Basic_action_a(
        SmId smId,                                            //!< The state machine id
        FppTest_SmInstanceInitial_Basic_Basic::Signal signal  //!< The signal
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

    //! The history associated with action a of basic1
    SmHarness::History<Basic_Basic::Signal, historySize> m_basic1_action_a_history;

    //! The history associated with action a of basic2
    SmHarness::History<Basic_Basic::Signal, historySize> m_basic2_action_a_history;

    //! The history associated with action a of smInitialBasic1
    SmHarness::History<SmInitial_Basic::Signal, historySize> m_smInitialBasic1_action_a_history;

    //! The history associated with action a of smInitialBasic2
    SmHarness::History<SmInitial_Basic::Signal, historySize> m_smInitialBasic2_action_a_history;
};

}  // namespace SmInstanceInitial

}  // namespace FppTest

#endif
