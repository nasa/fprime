// ======================================================================
// \title  WasmSequencerTester.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component test harness implementation class
// ======================================================================

#include "WasmSequencerTester.hpp"

#include "Fw/Port/OutputSerializePort.hpp"
#include "Os/FileSystem.hpp"

// Directory holding the committed golden WebAssembly modules. Injected by the
// unit-test CMake target (see Svc/WasmSequencer/CMakeLists.txt). The fallback
// keeps editors/linters that don't see the compile definition happy.
#ifndef WASM_SEQ_ASSET_DIR
#define WASM_SEQ_ASSET_DIR "."
#endif

namespace Svc {

// ----------------------------------------------------------------------
// Construction and destruction
// ----------------------------------------------------------------------

WasmSequencerTester ::WasmSequencerTester() : WasmSequencerTester(true) {}

WasmSequencerTester ::WasmSequencerTester(bool autoConfigure)
    : WasmSequencerGTestBase("WasmSequencerTester", WasmSequencerTester::MAX_HISTORY_SIZE),
      nextTlmId(0),
      nextPrmId(0),
      serialOutCount(0),
      lastSerialOutPort(0),
      lastSerialOutData{},
      lastSerialOutSize(0),
      seqStartOutCount(0),
      lastSeqStartFilename(""),
      lastSeqStartArgs(),
      seqDoneOutCount(0),
      lastSeqDoneResponse(Fw::CmdResponse::OK),
      component("WasmSequencer") {
    this->initComponents();
    this->connectPorts();

    // configure() allocates the component's backing pools and creates the initial interpreter store.
    // It runs after init() and port wiring (the interpreter SM's initial reset tolerates a null
    // store) and is REQUIRED before the component is exercised. A derived fixture may pass
    // autoConfigure=false to configure with a custom config per test instead.
    if (autoConfigure) {
        this->configureWith(TestConfig());
    }

    // Load parameters so INSTRUCTION_FUEL (and friends) take their declared
    // defaults. Without this the component's cached fuel is 0 and the interpreter
    // never makes progress (spins forever on OUT_OF_FUEL). Tests that need a
    // specific fuel value call paramSet_* and re-drive loading themselves.
    this->component.loadParameters();

    // configure() (via createStore) emits a construction-time StoreAllocationSucceeded event; reset
    // the history so each test starts from a clean baseline.
    this->clearHistory();
}

WasmSequencerTester ::~WasmSequencerTester() {
    this->component.deinit();
}

void WasmSequencerTester ::configureWith(const TestConfig& cfg) {
    WasmSequencer::SerialInQueueConfig serialInCfg;
    for (FwIndexType i = 0; i < WasmSequencer::NUM_SERIALIN_INPUT_PORTS; i++) {
        serialInCfg.sizes[i] = cfg.serialInSizes[i];
        serialInCfg.fullBehavior[i] = cfg.serialInBehaviors[i];
    }
    this->component.configure(cfg.dynPages, cfg.guestSize, cfg.stackSize, cfg.serialOutMax, serialInCfg,
                              this->m_allocator);
}

// ----------------------------------------------------------------------
// Golden-module helpers
// ----------------------------------------------------------------------

Fw::String WasmSequencerTester ::copyAsset(const char* name) {
    Fw::String src;
    src.format("%s/%s", WASM_SEQ_ASSET_DIR, name);

    // Destination is just the basename, in the current working directory, so it
    // fits within FW_CMD_STRING_MAX_SIZE when passed as a command argument.
    this->removeFile(name);
    const Os::FileSystem::Status status = Os::FileSystem::copyFile(src.toChar(), name);
    EXPECT_EQ(status, Os::FileSystem::Status::OP_OK)
        << "failed to stage golden module " << name << " from " << src.toChar();

    return Fw::String(name);
}

void WasmSequencerTester ::removeFile(const char* name) {
    // Best-effort: ignore "does not exist".
    (void)Os::FileSystem::removeFile(name);
}

void WasmSequencerTester ::flushTelemetry() {
    // The writeTelemetry port is an async scheduler tick; drive it and dispatch so the
    // handler runs and every tlmWrite_* lands in the tester history.
    this->clearTlm();
    this->invoke_to_writeTelemetry(0, 0);
    this->dispatchAll();
}

void WasmSequencerTester ::enqueueSerialIn(FwIndexType portNum, const U8* bytes, FwSizeType size) {
    Fw::LinearBufferTemplate<SERIAL_OUT_MAX_SIZE> msg(bytes, size);
    this->invoke_to_serialIn(portNum, msg);
    this->dispatchAll();
}

void WasmSequencerTester ::disconnectSerialOut(FwIndexType portNum) {
    // The framework offers no port disconnect and registerSerialPort() rejects nullptr, so
    // reconstruct the OutputSerializePort in place -- a fresh one has m_connObj == nullptr, so
    // isConnected() is false. Placement-new port reset is an established F´ UT idiom.
    Fw::OutputSerializePort& port = this->component.m_serialOut_OutputPort[portNum];
    port.~OutputSerializePort();
    new (&port) Fw::OutputSerializePort();
    port.init();
}

void WasmSequencerTester ::connectSerialOutTo(FwIndexType portNum, Fw::InputPortBase& port) {
    this->component.set_serialOut_OutputPort(portNum, &port);
}

void WasmSequencerTester ::unsetupSerialInQueue(FwIndexType portNum) {
    // Destroy and re-default-construct the CircularBuffer so it has no backing store (capacity 0),
    // as if configure() had been given size 0 for this port (setup() never called). The original
    // backing buffer is owned by the tester's allocator and freed at teardown, so this does not leak.
    Types::CircularBuffer& queue = this->component.m_serialInQueue[portNum];
    queue.~CircularBuffer();
    new (&queue) Types::CircularBuffer();
}

void WasmSequencerTester ::disconnectGetTlmChan(FwIndexType portNum) {
    Fw::OutputTlmGetPort& port = this->component.m_getTlmChan_OutputPort[portNum];
    port.~OutputTlmGetPort();
    new (&port) Fw::OutputTlmGetPort();
    port.init();
}

void WasmSequencerTester ::disconnectGetParam(FwIndexType portNum) {
    Fw::OutputPrmGetPort& port = this->component.m_getParam_OutputPort[portNum];
    port.~OutputPrmGetPort();
    new (&port) Fw::OutputPrmGetPort();
    port.init();
}

void WasmSequencerTester ::disconnectCmdOut(FwIndexType portNum) {
    Fw::OutputComPort& port = this->component.m_cmdOut_OutputPort[portNum];
    port.~OutputComPort();
    new (&port) Fw::OutputComPort();
    port.init();
}

Svc::SeqArgs WasmSequencerTester ::makeSeqArgs(const U8* bytes, FwSizeType size) {
    Svc::SeqArgs args;
    Svc::SeqArgs::Type_of_buffer buffer = {};
    FW_ASSERT(size <= sizeof buffer, static_cast<FwAssertArgType>(size));
    for (FwSizeType i = 0; i < size; i++) {
        buffer[i] = bytes[i];
    }
    args.set_size(size);
    args.set_buffer(buffer);
    return args;
}

// ----------------------------------------------------------------------
// Dispatch / state helpers
// ----------------------------------------------------------------------

FwSizeType WasmSequencerTester ::messagesAvailable() {
    return this->component.m_queue.getMessagesAvailable();
}

void WasmSequencerTester ::dispatchOne() {
    ASSERT_GT(this->messagesAvailable(), static_cast<FwSizeType>(0)) << "no queued message to dispatch";
    this->component.doDispatch();
}

void WasmSequencerTester ::dispatchAll(U32 bound) {
    U32 iters = 0;
    while (this->messagesAvailable() > 0 && iters < bound) {
        this->component.doDispatch();
        iters++;
    }
    ASSERT_LT(iters, bound) << "dispatchAll exceeded bound (livelock?)";
}

void WasmSequencerTester ::dispatchUntilControllerState(ControllerState state, U32 bound) {
    U32 iters = 0;
    while (this->controllerState() != state && iters < bound) {
        if (this->messagesAvailable() == 0) {
            break;
        }
        this->component.doDispatch();
        iters++;
    }
    ASSERT_EQ(this->controllerState(), state) << "controller did not reach expected state within bound";
}

void WasmSequencerTester ::dispatchUntilInterpreterState(InterpreterState state, U32 bound) {
    U32 iters = 0;
    while (this->interpreterState() != state && iters < bound) {
        if (this->messagesAvailable() == 0) {
            break;
        }
        this->component.doDispatch();
        iters++;
    }
    ASSERT_EQ(this->interpreterState(), state) << "interpreter did not reach expected state within bound";
}

WasmSequencerTester::ControllerState WasmSequencerTester ::controllerState() {
    return this->component.controller_getState();
}

WasmSequencerTester::InterpreterState WasmSequencerTester ::interpreterState() {
    return this->component.interpreter_getState();
}

U32 WasmSequencerTester ::lastCmdContext() {
    const U32 size = this->fromPortHistory_cmdOut->size();
    EXPECT_GT(size, 0u) << "no cmdOut dispatch recorded";
    return this->fromPortHistory_cmdOut->at(size - 1).context;
}

// ----------------------------------------------------------------------
// Canned telemetry / parameter responses
// ----------------------------------------------------------------------

Fw::TlmValid WasmSequencerTester ::from_getTlmChan_handler(FwIndexType portNum,
                                                           FwChanIdType id,
                                                           Fw::Time& timeTag,
                                                           Fw::TlmBuffer& val) {
    this->pushFromPortEntry_getTlmChan(id, timeTag, val);
    if (id != this->nextTlmId) {
        val.setBuffLen(0);
        return Fw::TlmValid::INVALID;
    }
    val = this->nextTlmValue;
    timeTag = this->nextTlmTime;
    return Fw::TlmValid::VALID;
}

Fw::ParamValid WasmSequencerTester ::from_getParam_handler(FwIndexType portNum, FwPrmIdType id, Fw::ParamBuffer& val) {
    this->pushFromPortEntry_getParam(id, val);
    if (id != this->nextPrmId) {
        return Fw::ParamValid::INVALID;
    }
    val = this->nextPrmValue;
    return Fw::ParamValid::VALID;
}

void WasmSequencerTester ::from_serialOut_handler(FwIndexType portNum, Fw::LinearBufferBase& buffer) {
    this->serialOutCount++;
    this->lastSerialOutPort = portNum;
    this->lastSerialOutSize = buffer.getSize();
    FW_ASSERT(this->lastSerialOutSize <= sizeof this->lastSerialOutData,
              static_cast<FwAssertArgType>(this->lastSerialOutSize));
    for (FwSizeType i = 0; i < this->lastSerialOutSize; i++) {
        this->lastSerialOutData[i] = buffer.getBuffAddr()[i];
    }
}

void WasmSequencerTester ::from_seqStartOut_handler(FwIndexType portNum,
                                                    const Fw::StringBase& filename,
                                                    const Svc::SeqArgs& args) {
    this->seqStartOutCount++;
    this->lastSeqStartFilename = filename;
    this->lastSeqStartArgs = args;
}

void WasmSequencerTester ::from_seqDoneOut_handler(FwIndexType portNum,
                                                   FwOpcodeType opCode,
                                                   U32 cmdSeq,
                                                   const Fw::CmdResponse& response) {
    this->seqDoneOutCount++;
    this->lastSeqDoneResponse = response;
}

}  // namespace Svc
