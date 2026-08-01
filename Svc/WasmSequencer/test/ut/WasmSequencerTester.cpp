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

void WasmSequencerTester ::dispatchUntilState(State state, U32 bound) {
    U32 iters = 0;
    while (this->getState() != state && iters < bound) {
        if (this->messagesAvailable() == 0) {
            break;
        }
        this->component.doDispatch();
        iters++;
    }
    ASSERT_EQ(this->getState(), state) << "did not reach expected state within bound";
}

WasmSequencerTester::State WasmSequencerTester ::getState() {
    return this->component.sequencer_getState();
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

}  // namespace Svc
