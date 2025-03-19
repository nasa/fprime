// ======================================================================
// \title  InputPairU16U32.hpp
// \author bocchino
// \brief  hpp file for InputPairU16U32 component implementation class
// ======================================================================

#ifndef FppTest_SmInstanceChoice_InputPairU16U32_HPP
#define FppTest_SmInstanceChoice_InputPairU16U32_HPP

#include "FppTest/state_machine/internal/harness/Harness.hpp"
#include "FppTest/state_machine/internal_instance/choice/InputPairU16U32ComponentAc.hpp"

namespace FppTest {

namespace SmInstanceChoice {

class InputPairU16U32 : public InputPairU16U32ComponentBase {
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

    //! The type FppTest_SmChoice_InputPairU16U32
    using SmChoice_InputPairU16U32 = FppTest_SmChoice_InputPairU16U32;

  public:
    // ----------------------------------------------------------------------
    // Component construction and destruction
    // ----------------------------------------------------------------------

    //! Construct InputPairU16U32 object
    InputPairU16U32(const char* const compName  //!< The component name
    );

    //! Destroy InputPairU16U32 object
    ~InputPairU16U32();

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine actions
    // ----------------------------------------------------------------------

    //! Implementation for action a of state machine FppTest_SmChoice_InputPairU16U32
    //!
    //! Action a
    void FppTest_SmChoice_InputPairU16U32_action_a(SmId smId,  //!< The state machine id
                                                   FppTest_SmChoice_InputPairU16U32::Signal signal,  //!< The signal
                                                   U32 value                                         //!< The value
                                                   ) override;

  PRIVATE:
    // ----------------------------------------------------------------------
    // Implementations for internal state machine guards
    // ----------------------------------------------------------------------

    //! Implementation for guard g of state machine FppTest_SmChoice_InputPairU16U32
    //!
    //! Guard g
    bool FppTest_SmChoice_InputPairU16U32_guard_g(SmId smId,  //!< The state machine id
                                                  FppTest_SmChoice_InputPairU16U32::Signal signal,  //!< The signal
                                                  U32 value                                         //!< The value
    ) const override;

  public:
    // ----------------------------------------------------------------------
    // Tests
    // ----------------------------------------------------------------------

    //! Run the test with signal s1 and true guard
    void testS1True();

    //! Run the test with signal s1 and false guard
    void testS1False();

    //! Run the test with signal s2 and true guard
    void testS2True();

    //! Run the test with signal s2 and false guard
    void testS2False();

  private:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The history associated with action a of smChoiceInputPairU16U32
    SmHarness::SignalValueHistory<SmChoice_InputPairU16U32::Signal, U32, historySize>
        m_smChoiceInputPairU16U32_action_a_history;

    //! The guard g of smChoiceInputPairU16U32
    SmHarness::Guard<SmChoice_InputPairU16U32::Signal, U32, historySize> m_smChoiceInputPairU16U32_guard_g;
};

}  // namespace SmInstanceChoice

}  // namespace FppTest

#endif
