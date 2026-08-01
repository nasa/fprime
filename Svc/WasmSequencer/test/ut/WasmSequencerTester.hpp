// ======================================================================
// \title  WasmSequencerTester.hpp
// \author tumbar
// \brief  hpp file for WasmSequencer component test harness implementation class
// ======================================================================

#ifndef Svc_WasmSequencerTester_HPP
#define Svc_WasmSequencerTester_HPP

#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencerGTestBase.hpp"

namespace Svc {

class WasmSequencerTester : public WasmSequencerGTestBase, public ::testing::Test {
  public:
    // ----------------------------------------------------------------------
    // Constants
    // ----------------------------------------------------------------------

    // Maximum size of histories storing events, telemetry, and port outputs
    static const FwSizeType MAX_HISTORY_SIZE = 100;

    // Instance ID supplied to the component instance under test
    static const FwEnumStoreType TEST_INSTANCE_ID = 0;

    // Queue depth supplied to the component instance under test
    static const FwSizeType TEST_INSTANCE_QUEUE_DEPTH = 20;

    static const FwOpcodeType OPCODE_RUN = WasmSequencer::OPCODE_RUN;
    static const FwOpcodeType OPCODE_WAIT = WasmSequencer::OPCODE_WAIT;
    static const FwOpcodeType OPCODE_LOAD = WasmSequencer::OPCODE_LOAD;
    static const FwOpcodeType OPCODE_LOAD_NAME = WasmSequencer::OPCODE_LOAD_NAME;
    static const FwOpcodeType OPCODE_INVOKE = WasmSequencer::OPCODE_INVOKE;
    static const FwOpcodeType OPCODE_CONTINUE = WasmSequencer::OPCODE_CONTINUE;

    //! Convenience alias for the sequencer state-machine state enum.
    using State = Svc::WasmSequencer_SequencerStateMachine_State;

  public:
    // ----------------------------------------------------------------------
    // Construction and destruction
    // ----------------------------------------------------------------------

    //! Construct object WasmSequencerTester
    WasmSequencerTester();

    //! Destroy object WasmSequencerTester
    ~WasmSequencerTester();

  protected:
    // ----------------------------------------------------------------------
    // Golden-module helpers
    // ----------------------------------------------------------------------

    //! Copy a committed golden module out of WASM_SEQ_ASSET_DIR into the test
    //! working directory under its short basename (F´ command string args are
    //! limited to FW_CMD_STRING_MAX_SIZE == 40 chars, so the full asset path
    //! cannot be passed to RUN/LOAD). Returns the basename to use as a file arg.
    Fw::String copyAsset(const char* name);

    //! Remove a file previously written into the working directory.
    void removeFile(const char* name);

    // ----------------------------------------------------------------------
    // Dispatch / state helpers
    // ----------------------------------------------------------------------

    //! Number of messages currently queued on the component.
    FwSizeType messagesAvailable();

    //! Dispatch exactly one queued message (asserts one was available).
    void dispatchOne();

    //! Drain every queued message, up to a bound (guards against livelock).
    void dispatchAll(U32 bound = 1000);

    //! Pump the queue until the component reaches `state` (or the bound/empty
    //! queue is hit). Asserts the state was reached.
    void dispatchUntilState(State state, U32 bound = 1000);

    //! Current sequencer state-machine state.
    State getState();

    // ----------------------------------------------------------------------
    // White-box accessors into the component under test
    // ----------------------------------------------------------------------

    U32 getPageUsedMask() const { return this->component.m_page_used_mask; }
    FwSizeType getGuestOffset() const { return this->component.m_guest_offset; }
    WasmSequencer_HostFunction getPendingHostFunctionKind() const { return this->component.m_pendingHostFunction.kind; }
    //! Forward to the component's private static mapping helpers (friendship
    //! does not extend to the generated TEST_F subclass, so expose them here).
    static Svc::WasmSequencer_TrapReason::T mapTrapReason(spacewasm_trap_t trap) {
        return WasmSequencer::mapTrapReason(trap);
    }

    bool hasPendingTimer() const { return this->component.m_hasPendingTimer; }
    bool hasPendingLoadCmd() const { return this->component.m_hasPendingLoadCmd; }
    bool pendingRun() const { return this->component.m_pendingRun; }

    //! Set the test-time returned by the component's timeCaller port.
    void setTestTime(const Fw::Time& time) { this->WasmSequencerGTestBase::setTestTime(time); }

    // ----------------------------------------------------------------------
    // Canned telemetry / parameter responses
    // ----------------------------------------------------------------------

    //! Handler for getTlmChan: returns nextTlm* when `id == nextTlmId`,
    //! otherwise reports the channel invalid.
    Fw::TlmValid from_getTlmChan_handler(FwIndexType portNum,
                                         FwChanIdType id,
                                         Fw::Time& timeTag,
                                         Fw::TlmBuffer& val) override;

    //! Handler for getParam: returns nextPrm* when `id == nextPrmId`, otherwise
    //! reports the parameter invalid.
    Fw::ParamValid from_getParam_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) override;

    FwChanIdType nextTlmId;
    Fw::Time nextTlmTime;
    Fw::TlmBuffer nextTlmValue;

    FwPrmIdType nextPrmId;
    Fw::ParamBuffer nextPrmValue;

  private:
    // ----------------------------------------------------------------------
    // Helper functions (definitions auto-generated by UT_AUTO_HELPERS)
    // ----------------------------------------------------------------------

    //! Connect ports
    void connectPorts();

    //! Initialize components
    void initComponents();

  protected:
    // ----------------------------------------------------------------------
    // Member variables
    // ----------------------------------------------------------------------

    //! The component under test
    WasmSequencer component;
};

}  // namespace Svc

#endif
