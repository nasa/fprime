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
    static const FwOpcodeType OPCODE_INVOKE = WasmSequencer::OPCODE_INVOKE;
    static const FwOpcodeType OPCODE_CONTINUE = WasmSequencer::OPCODE_CONTINUE;
    static const FwOpcodeType OPCODE_CANCEL = WasmSequencer::OPCODE_CANCEL;
    static const FwOpcodeType OPCODE_GLOBAL_SET_I32 = WasmSequencer::OPCODE_GLOBAL_SET_I32;
    static const FwOpcodeType OPCODE_GLOBAL_SET_I64 = WasmSequencer::OPCODE_GLOBAL_SET_I64;
    static const FwOpcodeType OPCODE_GLOBAL_SET_F32 = WasmSequencer::OPCODE_GLOBAL_SET_F32;
    static const FwOpcodeType OPCODE_GLOBAL_SET_F64 = WasmSequencer::OPCODE_GLOBAL_SET_F64;
    static const FwOpcodeType OPCODE_GLOBAL_GET = WasmSequencer::OPCODE_GLOBAL_GET;

    //! Convenience aliases for the two cooperating state-machine enums. The
    //! component is driven by a `controller` machine (load / validate /
    //! lifecycle) and an `interpreter` machine (instruction execution).
    //! Tests assert against whichever machine actually models the state under
    //! test rather than a synthesized single state.
    using ControllerState = Svc::WasmSequencer_ControllerStateMachine_State;
    using InterpreterState = Svc::WasmSequencer_InterpreterStateMachine_State;

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

    //! RAII helper: stages a golden module into the working directory on
    //! construction (copyAsset) and removes it on scope exit (removeFile).
    class StagedAsset {
      public:
        StagedAsset(WasmSequencerTester& tester, const char* name);
        ~StagedAsset();
        StagedAsset(const StagedAsset&) = delete;
        StagedAsset& operator=(const StagedAsset&) = delete;

        //! The basename to pass as the RUN/LOAD file argument.
        const Fw::String& file() const { return this->m_file; }

      private:
        WasmSequencerTester& m_tester;
        const char* m_name;
        Fw::String m_file;
    };

    //! Build a Svc::SeqArgs holding the given raw bytes (size + buffer), for driving
    //! the `args` host function round trip through RUN/INVOKE.
    static Svc::SeqArgs makeSeqArgs(const U8* bytes, FwSizeType size);

    //! Drive the writeTelemetry scheduler tick and dispatch it, flushing every
    //! telemetry channel into the tester history so ASSERT_TLM_* can read them.
    void flushTelemetry();

    //! Enqueue a raw serial message on serialIn[portNum] and dispatch it into the
    //! component's inbound queue. When called while the engine is not awaiting a serial
    //! message (e.g. IDLE), the resulting serialInMessage signal is a harmless no-op and
    //! the message simply waits in the queue for a later serial_recv to consume.
    void enqueueSerialIn(FwIndexType portNum, const U8* bytes, FwSizeType size);

    //! Reset serialOut[portNum] to an unconnected state (connectPorts() connects all ports
    //! via UT_AUTO_HELPERS; the framework has no disconnect, so reconstruct the port in
    //! place). Used to exercise the host's "port not connected" error path.
    void disconnectSerialOut(FwIndexType portNum);

    //! Re-point serialOut[portNum] at the given input port (e.g. a FailingSerialInputPort),
    //! so serialOut_out returns that port's invokeSerial status. Used to exercise the
    //! "serial port send failed" error path.
    void connectSerialOutTo(FwIndexType portNum, Fw::InputPortBase& port);

    //! Reset a plain (not-required) typed output port to an unconnected state (same
    //! placement-new idiom as disconnectSerialOut). Used to exercise the host's
    //! "output port not connected" guards for the tlm / prm / cmd host functions.
    void disconnectGetTlmChan(FwIndexType portNum);
    void disconnectGetParam(FwIndexType portNum);
    void disconnectCmdOut(FwIndexType portNum);

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

    //! Pump the queue until the interpreter machine reaches `state` (or
    //! the bound/empty queue is hit). Asserts the state was reached.
    void dispatchUntilInterpreterState(InterpreterState state, U32 bound = 1000);

    //! Current controller (load / validate / lifecycle) machine state.
    ControllerState controllerState();

    //! Current interpreter (instruction execution) machine state.
    InterpreterState interpreterState();

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
    // Event-assertion helpers
    // ----------------------------------------------------------------------

    //! Assert the total count of sequence-completion FAILURE events across the
    //! four distinct failure events (SequenceExited / SequencePanic /
    //! SequenceTrapped / SequenceHostFailure). Used where a test only cares that
    //! a failure occurred, independent of which branch reported it.
    void assertSequenceFailureCount(U32 expected) const {
        const U32 actual = static_cast<U32>(this->eventHistory_SequenceExited->size()) +
                           static_cast<U32>(this->eventHistory_SequencePanic->size()) +
                           static_cast<U32>(this->eventHistory_SequenceTrapped->size()) +
                           static_cast<U32>(this->eventHistory_SequenceHostFailure->size());
        ASSERT_EQ(expected, actual);
    }

    // ----------------------------------------------------------------------
    // White-box accessors into the component under test
    // ----------------------------------------------------------------------

    //! Whether page `i` of the global page pool is currently handed out (white-box).
    bool isPageUsed(FwSizeType i) const { return this->component.m_page_used[i]; }
    FwSizeType getGuestOffset() const { return this->component.m_guest_pool_offset; }
    //! Access the inbound serial queue for a given port index (white-box). Friendship is not
    //! inherited by the generated TEST_F subclass, so expose it through the tester.
    Types::CircularBuffer& serialInQueue(FwIndexType portNum) { return this->component.m_serialInQueue[portNum]; }
    WasmSequencer_HostFunction getPendingHostFunctionKind() const { return this->component.m_pendingHostFunction.kind; }
    //! Forward to the component's private static mapping helpers (friendship
    //! does not extend to the generated TEST_F subclass, so expose them here).
    static Svc::WasmSequencer_TrapReason::T mapTrapReason(spacewasm_trap_t trap) {
        return WasmSequencer::mapTrapReason(trap);
    }

    bool hasPendingTimer() const { return this->component.m_hasPendingTimer; }

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

    //! Handler for seqStartOut: records the run-start report the component emits
    //! to internal callers. Captured into dedicated members (rather than the
    //! shared from-port history) so the many ASSERT_FROM_PORT_HISTORY_SIZE(0)
    //! guards in existing tests keep working; the byte-level round trip is
    //! asserted through these accessors instead.
    void from_seqStartOut_handler(FwIndexType portNum,
                                  const Fw::StringBase& filename,
                                  const Svc::SeqArgs& args) override;

    //! Handler for seqDoneOut: records the run-done report the component emits to
    //! internal callers. See from_seqStartOut_handler for why this is captured
    //! outside the shared from-port history.
    void from_seqDoneOut_handler(FwIndexType portNum,
                                 FwOpcodeType opCode,
                                 U32 cmdSeq,
                                 const Fw::CmdResponse& response) override;

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

    //! Number of seqStartOut invocations observed, and the arguments of the last one.
    U32 seqStartOutCount;
    Fw::String lastSeqStartFilename;
    Svc::SeqArgs lastSeqStartArgs;

    //! Number of seqDoneOut invocations observed, and the response of the last one.
    U32 seqDoneOutCount;
    Fw::CmdResponse lastSeqDoneResponse;

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

// Defined out of line so the enclosing tester is a complete type when the
// constructor calls its (protected) copyAsset/removeFile helpers.
inline WasmSequencerTester::StagedAsset::StagedAsset(WasmSequencerTester& tester, const char* name)
    : m_tester(tester), m_name(name), m_file(tester.copyAsset(name)) {}

inline WasmSequencerTester::StagedAsset::~StagedAsset() {
    this->m_tester.removeFile(this->m_name);
}

}  // namespace Svc

#endif
