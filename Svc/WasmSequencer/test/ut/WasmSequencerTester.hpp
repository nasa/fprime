// ======================================================================
// \title  WasmSequencerTester.hpp
// \author tumbar
// \brief  hpp file for WasmSequencer component test harness implementation class
// ======================================================================

#ifndef Svc_WasmSequencerTester_HPP
#define Svc_WasmSequencerTester_HPP

#include "Fw/Types/MallocAllocator.hpp"
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

    // ----------------------------------------------------------------------
    // Backing-store sizes handed to component.configure() in the tester ctor:
    // dynamic pool = 4 * SPACEWASM_PAGE_SIZE, guest pool = 2048 B, Wasm stack =
    // 256 B, serialOut buffer = 256 B, each serialIn queue = 256 B / DROP_OLDEST.
    // ----------------------------------------------------------------------
    static const FwSizeType DYNAMIC_POOL_PAGES = 4;
    static const FwSizeType GUEST_POOL_SIZE = 2048;
    static const FwSizeType WASM_STACK_SIZE = 256;
    static const U32 SERIAL_OUT_MAX_SIZE = 256;
    static const FwSizeType SERIAL_IN_QUEUE_SIZE = 256;

    static const FwOpcodeType OPCODE_RUN = WasmSequencer::OPCODE_RUN;
    static const FwOpcodeType OPCODE_WAIT = WasmSequencer::OPCODE_WAIT;
    static const FwOpcodeType OPCODE_LOAD = WasmSequencer::OPCODE_LOAD;
    static const FwOpcodeType OPCODE_INVOKE = WasmSequencer::OPCODE_INVOKE;
    static const FwOpcodeType OPCODE_CONTINUE = WasmSequencer::OPCODE_CONTINUE;
    static const FwOpcodeType OPCODE_CANCEL = WasmSequencer::OPCODE_CANCEL;
    static const FwOpcodeType OPCODE_PAUSE = WasmSequencer::OPCODE_PAUSE;
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
    // Configuration (multi-config support)
    // ----------------------------------------------------------------------

    //! Delegated-to constructor. When autoConfigure is false the component is initialized and wired
    //! but NOT configured, so a derived fixture (WasmSequencerConfigTester) can drive
    //! component.configure() with a custom config per test to exercise per-config branches and
    //! misconfigurations. The event history is cleared either way.
    explicit WasmSequencerTester(bool autoConfigure);

    //! Build a WasmSequencer::Config pre-populated with the tester's standard sizing (the defaults
    //! used by the auto-configuring fixture). Tests that need a variant start from this and tweak
    //! individual fields.
    WasmSequencer::Config standardConfig() const;

    //! Drive component.configure() from a WasmSequencer::Config (using the tester's tracking allocator).
    void configureWith(const WasmSequencer::Config& cfg);

    //! Whether the component currently owns an interpreter store (white-box; configure() creates it).
    bool isStoreCreated() const { return this->component.m_wasm != nullptr; }

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

    //! Number of dynamic (interpreter-heap) pages currently handed out by the page allocator (white-box).
    FwSizeType dynamicPagesUsed() const { return this->component.m_heapPagesUsed; }
    //! Total number of dynamic pages the pool was configured with (white-box).
    FwSizeType dynamicPagesCapacity() const { return this->component.m_config.heapPages; }
    //! Current bump offset into the guest linear-memory pool (white-box).
    FwSizeType getGuestOffset() const { return this->component.m_guestPoolOffset; }
    //! Total size of the guest linear-memory pool (white-box).
    FwSizeType guestPoolSize() const { return this->component.m_config.guestMemorySize; }
    //! Drive the guest bump allocator directly (the callbacks are private). Used by the memory.grow
    //! (guestRealloc) tests to exercise the grow logic without a live spacewasm store.
    U8* wbGuestAlloc(U32 size, U32 align) { return this->component.guestAlloc(size, align); }
    U8* wbGuestRealloc(U8* ptr, FwSizeType oldSize, FwSizeType newSize, U32 align) {
        return this->component.guestRealloc(ptr, oldSize, newSize, align);
    }
    //! Drive the raw spacewasm realloc callback (with size_t sizes) to exercise the truncation guard.
    U8* wbGuestReallocCallback(U8* ptr, size_t oldSize, size_t newSize, size_t align) {
        return WasmSequencer::guestReallocCallback(&this->component, ptr, oldSize, newSize, align);
    }
    //! White-box: simulate the spacewasm page allocator freeing a live page and then requesting a new
    //! one within the same store lifetime. The dynamic-page allocator is a bump index guarded by a
    //! poison flag: any free poisons the pool so a subsequent alloc fails fast rather than possibly
    //! aliasing a still-live page (spacewasm frees pages in forward-scan, not LIFO, order). Intended
    //! for a death test -- it deliberately trips the guard.
    void pokePageFreeThenReAlloc() {
        FW_ASSERT(this->component.m_heapPagesUsed > 0);
        this->component.globalDealloc(this->component.m_heapPages[0]);
        (void)this->component.globalAlloc(static_cast<U32>(Svc::WasmSequencerConfig::SPACEWASM_PAGE_SIZE), 8u);
    }
    //! Access the inbound serial queue for a given port index (white-box). Friendship is not
    //! inherited by the generated TEST_F subclass, so expose it through the tester.
    Types::CircularBuffer& serialInQueue(FwIndexType portNum) { return this->component.m_serialInQueue[portNum]; }
    //! Override the configured queue-full behavior for a serialIn port index (white-box). The tester
    //! ctor configures every port with DROP_OLDEST; tests that exercise DROP_NEWEST/ASSERT set it here.
    void setSerialInFullBehavior(FwIndexType portNum, WasmSequencer::SerialInQueueFullBehavior behavior) {
        this->component.m_config.serialIn[portNum].fullBehavior = behavior;
    }
    //! Reset a serialIn queue to the un-setup (no backing buffer, capacity 0) state, modeling a port
    //! that configure() was given size 0. Placement-new reset mirrors the disconnectSerialOut idiom.
    void unsetupSerialInQueue(FwIndexType portNum);
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
    U8 lastSerialOutData[SERIAL_OUT_MAX_SIZE];
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

    //! Allocator backing the component's configure() pools
    Fw::MallocAllocator m_allocator;

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

//! Fixture variant that does NOT auto-configure the component, so each test drives
//! component.configure() with its own WasmSequencer::Config (via standardConfig()). Used to
//! exercise per-config branches
//! (serialIn sizes/behaviors, serialOut buffer sizing) and misconfigurations (assert paths).
class WasmSequencerConfigTester : public WasmSequencerTester {
  public:
    WasmSequencerConfigTester() : WasmSequencerTester(/*autoConfigure=*/false) {}
    // No custom teardown needed: the component destructor tolerates a never-configured instance
    // (its m_wasm / m_allocator null guards skip destroyStore and deallocate). So a configure-failure
    // death test, whose failing configure runs only in a forked child, leaves the parent's component
    // unconfigured and it tears down cleanly -- exercising those null guards.
};

}  // namespace Svc

#endif
