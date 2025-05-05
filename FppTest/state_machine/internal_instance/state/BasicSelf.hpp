// ======================================================================
// \title  BasicSelf.hpp
// \author bocchino
// \brief  hpp file for BasicSelf component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceState_BasicSelf_HPP
#define FppTest_SmInstanceState_BasicSelf_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/state/BasicSelfComponentAc.hpp"

namespace FppTest {

namespace SmInstanceState {

class BasicSelf : public BasicSelfComponentBase {
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

    //! The type FppTest_SmState_BasicSelf
    using SmState_BasicSelf = FppTest_SmState_BasicSelf;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct BasicSelf object
    BasicSelf(const char* const compName  //!< The component name
    );

    //! Destroy BasicSelf object
    ~BasicSelf();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmState_BasicSelf
    //!
    //! Action a
    void FppTest_SmState_BasicSelf_action_a(SmId smId,                                //!< The state machine id
                                            FppTest_SmState_BasicSelf::Signal signal  //!< The signal
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

    //! The history associated with action a of smStateBasicSelf
    SmHarness::History<SmState_BasicSelf::Signal, historySize> m_smStateBasicSelf_action_a_history;
};

}  // namespace SmInstanceState

}  // namespace FppTest

#endif
