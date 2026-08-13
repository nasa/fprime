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
    static const FwOpcodeType OPCODE_CANCEL = WasmSequencer::OPCODE_CANCEL;

    //! Convenience aliases for the two cooperating state-machine enums. The
    //! component is driven by a `controller` machine (load / validate /
    //! lifecycle) and an `interpreter` engine machine (instruction execution).
    //! Tests assert against whichever machine actually models the state under
    //! test rather than a synthesized single state.
    using ControllerState = Svc::WasmSequencer_ControllerStateMachine_State;
    using EngineState = Svc::WasmSequencer_EngineStateMachine_State;

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

    //! Build a Svc::SeqArgs holding the given raw bytes (size + buffer), for driving
    //! the `args` host function round trip through RUN/INVOKE.
    static Svc::SeqArgs makeSeqArgs(const U8* bytes, FwSizeType size);

    //! Drive the writeTelemetry scheduler tick and dispatch it, flushing every
    //! telemetry channel into the tester history so ASSERT_TLM_* can read them.
    void flushTelemetry();

    // ----------------------------------------------------------------------
    // Dispatch / state helpers
    // ----------------------------------------------------------------------

    //! Number of messages currently queued on the component.
    FwSizeType messagesAvailable();

    //! Dispatch exactly one queued message (asserts one was available).
    void dispatchOne();

    //! Drain every queued message, up to a bound (guards against livelock).
    void dispatchAll(U32 bound = 1000);

    //! Pump the queue until the controller machine reaches `state` (or the
    //! bound/empty queue is hit). Asserts the state was reached.
    void dispatchUntilControllerState(ControllerState state, U32 bound = 1000);

    //! Pump the queue until the interpreter engine machine reaches `state` (or
    //! the bound/empty queue is hit). Asserts the state was reached.
    void dispatchUntilEngineState(EngineState state, U32 bound = 1000);

    //! Current controller (load / validate / lifecycle) machine state.
    ControllerState controllerState();

    //! Current interpreter engine (instruction execution) machine state.
    EngineState engineState();

    //! Context (cmdUid) attached to the most recent cmdOut dispatch. Tests echo
    //! this back as the cmdSeq argument of cmdResponseIn so the component accepts
    //! the response as belonging to the current command instance.
    U32 lastCmdContext();

    //! The cmdUid the component would currently generate (current sequence +
    //! command counters). Used by tests that inject a response tagged with the
    //! current sequence index without a command actually being dispatched, to
    //! exercise the "unexpected response" failure paths.
    U32 currentCmdUid() const { return this->component.makeCmdUid(); }

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

    //! Handler for serialOut: records the last port number and payload bytes so tests can
    //! assert the guest's serial invocation round-tripped verbatim.
    void from_serialOut_handler(FwIndexType portNum, Fw::LinearBufferBase& buffer) override;

    FwChanIdType nextTlmId;
    Fw::Time nextTlmTime;
    Fw::TlmBuffer nextTlmValue;

    FwPrmIdType nextPrmId;
    Fw::ParamBuffer nextPrmValue;

    //! Number of serialOut invocations observed
    U32 serialOutCount;
    //! Port number and payload captured from the most recent serialOut invocation
    FwIndexType lastSerialOutPort;
    U8 lastSerialOutData[Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE];
    FwSizeType lastSerialOutSize;

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
