// ======================================================================
// \title  WasmSequencerTester.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component test harness implementation class
// ======================================================================

#include "WasmSequencerTester.hpp"

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

WasmSequencerTester ::WasmSequencerTester()
    : WasmSequencerGTestBase("WasmSequencerTester", WasmSequencerTester::MAX_HISTORY_SIZE),
      nextTlmId(0),
      nextPrmId(0),
      serialOutCount(0),
      lastSerialOutPort(0),
      lastSerialOutData{},
      lastSerialOutSize(0),
      component("WasmSequencer") {
    this->initComponents();
    this->connectPorts();

    // Load parameters so INSTRUCTION_FUEL (and friends) take their declared
    // defaults. Without this the component's cached fuel is 0 and the interpreter
    // never makes progress (spins forever on OUT_OF_FUEL). Tests that need a
    // specific fuel value call paramSet_* and re-drive loading themselves.
    this->component.loadParameters();
}

WasmSequencerTester ::~WasmSequencerTester() {
    this->component.deinit();
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
    EXPECT_EQ(status, Os::FileSystem::Status::OP_OK) << "failed to stage golden module " << name << " from " << src.toChar();

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

void WasmSequencerTester ::dispatchUntilEngineState(EngineState state, U32 bound) {
    U32 iters = 0;
    while (this->engineState() != state && iters < bound) {
        if (this->messagesAvailable() == 0) {
            break;
        }
        this->component.doDispatch();
        iters++;
    }
    ASSERT_EQ(this->engineState(), state) << "engine did not reach expected state within bound";
}

WasmSequencerTester::ControllerState WasmSequencerTester ::controllerState() {
    return this->component.controller_getState();
}

WasmSequencerTester::EngineState WasmSequencerTester ::engineState() {
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

}  // namespace Svc
