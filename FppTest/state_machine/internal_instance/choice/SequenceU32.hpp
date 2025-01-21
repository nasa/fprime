// ======================================================================
// \title  SequenceU32.hpp
// \author bocchino
// \brief  hpp file for SequenceU32 component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceChoice_SequenceU32_HPP
#define FppTest_SmInstanceChoice_SequenceU32_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/choice/SequenceU32ComponentAc.hpp"

namespace FppTest {

namespace SmInstanceChoice {

class SequenceU32 : public SequenceU32ComponentBase {
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

    //! The type FppTest_SmChoice_SequenceU32
    using SmChoice_SequenceU32 = FppTest_SmChoice_SequenceU32;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct SequenceU32 object
    SequenceU32(const char* const compName  //!< The component name
    );

    //! Destroy SequenceU32 object
    ~SequenceU32();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmChoice_SequenceU32
    //!
    //! Action a
    void FppTest_SmChoice_SequenceU32_action_a(SmId smId,                                    //!< The state machine id
                                               FppTest_SmChoice_SequenceU32::Signal signal,  //!< The signal
                                               U32 value                                     //!< The value
                                               ) override;

    //! Implementation for action b of state machine FppTest_SmChoice_SequenceU32
    //!
    //! Action b
    void FppTest_SmChoice_SequenceU32_action_b(SmId smId,                                   //!< The state machine id
                                               FppTest_SmChoice_SequenceU32::Signal signal  //!< The signal
                                               ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g1 of state machine FppTest_SmChoice_SequenceU32
    //!
    //! Guard g1
    bool FppTest_SmChoice_SequenceU32_guard_g1(SmId smId,                                   //!< The state machine id
                                               FppTest_SmChoice_SequenceU32::Signal signal  //!< The signal
    ) const override;

    //! Implementation for guard g2 of state machine FppTest_SmChoice_SequenceU32
    //!
    //! Guard g2
    bool FppTest_SmChoice_SequenceU32_guard_g2(SmId smId,                                    //!< The state machine id
                                               FppTest_SmChoice_SequenceU32::Signal signal,  //!< The signal
                                               U32 value                                     //!< The value
    ) const override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the test with g1 true
    void testG1True();

    //! Run the test with g1 true and g2 true
    void testG1FalseG2True();

    //! Run the test with g1 true and g2 false
    void testG1FalseG2False();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of smChoiceSequenceU32
    SmHarness::SignalValueHistory<SmChoice_SequenceU32::Signal, U32, historySize>
        m_smChoiceSequenceU32_action_a_history;

    //! The history associated with action b of smChoiceSequenceU32
    SmHarness::History<SmChoice_SequenceU32::Signal, historySize> m_smChoiceSequenceU32_action_b_history;

    //! The guard g1 of smChoiceSequenceU32
    SmHarness::NoArgGuard<SmChoice_SequenceU32::Signal, historySize> m_smChoiceSequenceU32_guard_g1;

    //! The guard g2 of smChoiceSequenceU32
    SmHarness::Guard<SmChoice_SequenceU32::Signal, U32, historySize> m_smChoiceSequenceU32_guard_g2;
};

}  // namespace SmInstanceChoice

}  // namespace FppTest

#endif
