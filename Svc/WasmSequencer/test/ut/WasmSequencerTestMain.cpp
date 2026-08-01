// ======================================================================
// \title  WasmSequencerTestMain.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component test main function
//
// The tests drive the component through its public command interface and
// pump its message queue (dispatchAll / dispatchUntilState) exactly as the
// active task would, then assert on the resulting state, emitted events, and
// output-port invocations. Guest behavior is supplied by the committed golden
// WebAssembly modules under test/wasm (see that directory's README).
// ======================================================================

#include "WasmSequencerGTestBase.hpp"
#include "WasmSequencerTester.hpp"

#include "WasmSequencer_SequencerStateMachine_StateEnumAc.hpp"
#include "config/FwPacketDescriptorTypeAliasAc.h"

namespace Svc {

using State = WasmSequencerTester::State;

// Convenience: the two block modes.
static const Svc::BlockState BLOCK(Svc::BlockState::BLOCK);
static const Svc::BlockState NO_BLOCK(Svc::BlockState::NO_BLOCK);

// ----------------------------------------------------------------------
// Construction / initial state
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, InitialStateIsIdle) {
    ASSERT_EQ(this->getState(), State::IDLE);
    // No pending work on a fresh component.
    ASSERT_FALSE(this->hasPendingLoadCmd());
    ASSERT_FALSE(this->hasPendingTimer());
    ASSERT_FALSE(this->pendingRun());
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::NONE);
}

// ----------------------------------------------------------------------
// LOAD / LOAD_NAME (nominal + failures)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, LoadEmptyModuleReady) {
    const Fw::String file = this->copyAsset("empty.wasm");

    this->sendCmd_LOAD(0, 10, file);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 10, Fw::CmdResponse::OK);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, LoadNamedModuleReady) {
    const Fw::String file = this->copyAsset("empty.wasm");

    this->sendCmd_LOAD_NAME(0, 11, file, Fw::CmdStringArg("mod"));
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD_NAME, 11, Fw::CmdResponse::OK);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, LoadFileNotFound) {
    // No copyAsset: the file simply does not exist in the CWD.
    this->removeFile("does_not_exist.wasm");
    this->sendCmd_LOAD(0, 12, Fw::CmdStringArg("does_not_exist.wasm"));
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    // Open failure surfaces as FileOpenError (naming the missing file) then a
    // failed load. The status arg is the Os::File open status; assert only the
    // file name here since the numeric status is platform-dependent.
    ASSERT_EVENTS_FileOpenError_SIZE(1);
    ASSERT_STREQ(this->eventHistory_FileOpenError->at(0).fileName.toChar(), "does_not_exist.wasm");
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 12, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(WasmSequencerTester, LoadMalformedMagic) {
    const Fw::String file = this->copyAsset("malformed.wasm");
    this->sendCmd_LOAD(0, 13, file);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // A bad wasm magic number decodes to ERR_MALFORMED_MAGIC.
    ASSERT_EVENTS_ModuleLoadFailed(0, WasmSequencer_Status::ERR_MALFORMED_MAGIC);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 13, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("malformed.wasm");
}

TEST_F(WasmSequencerTester, LoadTruncated) {
    const Fw::String file = this->copyAsset("truncated.wasm");
    this->sendCmd_LOAD(0, 14, file);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    this->removeFile("truncated.wasm");
}

TEST_F(WasmSequencerTester, LoadBigMemFailsAllocator) {
    // Requests more guest pages than the 2048-byte guest pool can back.
    const Fw::String file = this->copyAsset("bigmem.wasm");
    this->sendCmd_LOAD(0, 15, file);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // A failed load must return the guest allocator to empty (no fragmentation).
    ASSERT_EQ(this->getGuestOffset(), static_cast<FwSizeType>(0));
    this->removeFile("bigmem.wasm");
}

TEST_F(WasmSequencerTester, LoadBadImportNameFails) {
    const Fw::String file = this->copyAsset("bad_import_name.wasm");
    this->sendCmd_LOAD(0, 16, file);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    this->removeFile("bad_import_name.wasm");
}

TEST_F(WasmSequencerTester, LoadBadImportModuleFails) {
    const Fw::String file = this->copyAsset("bad_import_module.wasm");
    this->sendCmd_LOAD(0, 17, file);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    this->removeFile("bad_import_module.wasm");
}

TEST_F(WasmSequencerTester, LoadBadImportSigFails) {
    const Fw::String file = this->copyAsset("bad_import_sig.wasm");
    this->sendCmd_LOAD(0, 18, file);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    this->removeFile("bad_import_sig.wasm");
}

// ----------------------------------------------------------------------
// RUN (nominal completion, start functions, traps)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RunEmptyNoBlock) {
    const Fw::String file = this->copyAsset("empty.wasm");

    // NO_BLOCK responds OK once the module loads; the program then runs to
    // completion and lands in READY.
    this->sendCmd_RUN(0, 20, file, NO_BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 20, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, RunEmptyBlock) {
    const Fw::String file = this->copyAsset("empty.wasm");

    // BLOCK holds the command response until the interpreter finishes.
    this->sendCmd_RUN(0, 21, file, BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 21, Fw::CmdResponse::OK);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, RunStartModule) {
    // Module with a `start` function that runs at instantiation.
    const Fw::String file = this->copyAsset("start.wasm");

    this->sendCmd_RUN(0, 22, file, BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 22, Fw::CmdResponse::OK);
    this->removeFile("start.wasm");
}

TEST_F(WasmSequencerTester, RunNoMainFailsInvoke) {
    // Valid module that exports `other` but not `main`.
    const Fw::String file = this->copyAsset("no_main.wasm");

    this->sendCmd_RUN(0, 23, file, BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_ModuleInvokeFailed_SIZE(1);
    // No `main` export resolves to ERR_NOT_FOUND.
    ASSERT_EVENTS_ModuleInvokeFailed(0, WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 23, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("no_main.wasm");
}

TEST_F(WasmSequencerTester, RunUnreachableTraps) {
    const Fw::String file = this->copyAsset("unreachable.wasm");

    this->sendCmd_RUN(0, 24, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::UNREACHABLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 24, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("unreachable.wasm");
}

TEST_F(WasmSequencerTester, RunDivZeroTraps) {
    const Fw::String file = this->copyAsset("divzero.wasm");

    this->sendCmd_RUN(0, 25, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::DIVIDE_BY_ZERO);
    this->removeFile("divzero.wasm");
}

TEST_F(WasmSequencerTester, RunExitTraps) {
    const Fw::String file = this->copyAsset("exit.wasm");

    this->sendCmd_RUN(0, 26, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    // A host function returning SPACEWASM_TRAP surfaces as the HOST trap reason.
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::HOST);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 26, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("exit.wasm");
}

TEST_F(WasmSequencerTester, RunPanicTraps) {
    const Fw::String file = this->copyAsset("panic.wasm");

    this->sendCmd_RUN(0, 27, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    // fprime.panic returns SPACEWASM_TRAP, surfacing as the HOST trap reason.
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::HOST);
    this->removeFile("panic.wasm");
}

TEST_F(WasmSequencerTester, RunStartTrapsToIdle) {
    // A module whose `start` function contains `unreachable`. The interpreter
    // begins the start function (startInvoked -> RUNNING) and traps while
    // spinning, surfacing as a SequenceTrap and returning to IDLE with an
    // EXECUTION_ERROR response.
    const Fw::String file = this->copyAsset("start_trap.wasm");

    this->sendCmd_RUN(0, 28, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::UNREACHABLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 28, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("start_trap.wasm");
}

// ----------------------------------------------------------------------
// INVOKE
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, InvokeAfterLoad) {
    const Fw::String file = this->copyAsset("empty.wasm");

    // Load (unnamed) then invoke "main" of the unnamed module.
    this->sendCmd_LOAD(0, 30, file);
    this->dispatchUntilState(State::READY);
    ASSERT_EQ(this->getState(), State::READY);

    // Already READY, so pump the queue to completion rather than waiting on a
    // state change (INVOKE of a trivial main returns straight back to READY).
    this->sendCmd_INVOKE(0, 31, Fw::CmdStringArg(""), BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 31, Fw::CmdResponse::OK);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, InvokeNoBlockRespondsImmediately) {
    const Fw::String file = this->copyAsset("empty.wasm");

    this->sendCmd_LOAD(0, 35, file);
    this->dispatchUntilState(State::READY);

    // NO_BLOCK INVOKE responds OK as soon as the module resolves, before the
    // function actually runs (INVOKE_cmdHandler NO_BLOCK path).
    this->sendCmd_INVOKE(0, 36, Fw::CmdStringArg(""), NO_BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 36, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, InvokeUnknownModule) {
    const Fw::String file = this->copyAsset("empty.wasm");

    this->sendCmd_LOAD(0, 32, file);
    this->dispatchUntilState(State::READY);

    // A module name that was never loaded resolves to not-found.
    this->sendCmd_INVOKE(0, 33, Fw::CmdStringArg("nope"), NO_BLOCK);
    this->dispatchAll();

    ASSERT_EVENTS_ModuleNotFound_SIZE(1);
    ASSERT_EVENTS_ModuleNotFound(0, "nope");
    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 33, Fw::CmdResponse::EXECUTION_ERROR);
    // State is unchanged (still READY).
    ASSERT_EQ(this->getState(), State::READY);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, InvokeFromIdleInvalid) {
    // INVOKE is only valid from READY.
    this->sendCmd_INVOKE(0, 34, Fw::CmdStringArg(""), NO_BLOCK);
    this->dispatchAll();

    ASSERT_EVENTS_InvalidCommand_SIZE(1);
    ASSERT_EVENTS_InvalidCommand(0, WasmSequencer_SequencerStateMachine_State::IDLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_INVOKE, 34, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EQ(this->getState(), State::IDLE);
}

// ----------------------------------------------------------------------
// WAIT
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, WaitFromIdleRespondsImmediately) {
    this->sendCmd_WAIT(0, 40);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(0, OPCODE_WAIT, 40, Fw::CmdResponse::OK);
    ASSERT_EQ(this->getState(), State::IDLE);
}

TEST_F(WasmSequencerTester, WaitFromReadyRespondsImmediately) {
    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_LOAD(0, 41, file);
    this->dispatchUntilState(State::READY);

    this->sendCmd_WAIT(0, 42);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(1, OPCODE_WAIT, 42, Fw::CmdResponse::OK);
    // WAIT from READY responds immediately without changing state.
    ASSERT_EQ(this->getState(), State::READY);
    this->removeFile("empty.wasm");
}

// ----------------------------------------------------------------------
// Invalid-state command rejections
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ContinueFromIdleInvalid) {
    this->sendCmd_CONTINUE(0, 50);
    this->dispatchAll();
    ASSERT_EVENTS_InvalidCommand_SIZE(1);
    ASSERT_EVENTS_InvalidCommand(0, WasmSequencer_SequencerStateMachine_State::IDLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_CONTINUE, 50, Fw::CmdResponse::EXECUTION_ERROR);
    // The rejected command leaves the component in IDLE.
    ASSERT_EQ(this->getState(), State::IDLE);
}

// ----------------------------------------------------------------------
// Static mapping helpers (direct unit tests)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, MapTrapReasonAllCases) {
    using TR = WasmSequencer_TrapReason;
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_UNREACHABLE), TR::UNREACHABLE);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_HOST), TR::HOST);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_DIVIDE_BY_ZERO), TR::DIVIDE_BY_ZERO);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_INVALID_TABLE_INDEX), TR::INVALID_TABLE_INDEX);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_INVALID_TABLE_FUNCTION_TYPE), TR::INVALID_TABLE_FUNCTION_TYPE);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_UNINITIALIZED_TABLE_ELEMENT), TR::UNINTIIALIZED_TABLE_ELEMENT);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_GLOBAL_GET_FAILED), TR::GLOBAL_GET_FAILED);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_GLOBAL_SET_FAILED), TR::GLOBAL_SET_FAILED);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_OUT_OF_MEMORY), TR::OUT_OF_MEMORY);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_MEMORY_REF_NOT_UNIQUE), TR::MEMORY_REF_NOT_UNIQUE);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_MEMORY_OUT_OF_BOUNDS), TR::MEMORY_OUT_OF_BOUNDS);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_STACK_OVERFLOW), TR::STACK_OVERFLOW);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_UNREPRESENTABLE_RESULT), TR::UNREPRESENTABLE_RESULT);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_INTEGER_OVERFLOW), TR::INTEGER_OVERFLOW);
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_BAD_CONVERSION_TO_INTEGER), TR::BAD_CONVERSION_TO_INTEGER);
    // Anything unmapped (e.g. TRAP_NONE) falls through to HOST.
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_NONE), TR::HOST);
}

// ----------------------------------------------------------------------
// Host functions: EVENT
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, EventActivityHi) {
    const Fw::String file = this->copyAsset("event.wasm");

    this->sendCmd_RUN(0, 60, file, BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_LogActivityHi_SIZE(1);
    ASSERT_EVENTS_LogActivityHi(0, "hello wasm");
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("event.wasm");
}

TEST_F(WasmSequencerTester, EventAllSeverities) {
    const Fw::String file = this->copyAsset("event_all_sev.wasm");

    this->sendCmd_RUN(0, 61, file, BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    // One event emitted per severity 1..7.
    ASSERT_EVENTS_LogFatal_SIZE(1);
    ASSERT_EVENTS_LogWarningHi_SIZE(1);
    ASSERT_EVENTS_LogWarningLo_SIZE(1);
    ASSERT_EVENTS_LogCommand_SIZE(1);
    ASSERT_EVENTS_LogActivityHi_SIZE(1);
    ASSERT_EVENTS_LogActivityLo_SIZE(1);
    ASSERT_EVENTS_LogDiagnostic_SIZE(1);
    this->removeFile("event_all_sev.wasm");
}

TEST_F(WasmSequencerTester, EventMessageTruncatedToMax) {
    // The guest passes a 250-byte message length; the host clamps it to
    // FW_LOG_STRING_MAX_SIZE (200) before reading (wasmEvent len clamp). The
    // emitted event string is exactly 200 'A' characters.
    const Fw::String file = this->copyAsset("event_toobig.wasm");

    this->sendCmd_RUN(0, 63, file, BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_LogActivityHi_SIZE(1);

    // The host reads at most FW_LOG_STRING_MAX_SIZE (200) bytes; the LogActivityHi
    // event string arg further caps at its declared `string size 128`. So the
    // observed message is 128 'A' characters -- all identical, so truncation at
    // either bound yields the same run of 'A's, confirming the clamp fired.
    const std::string expected(128, 'A');
    ASSERT_EVENTS_LogActivityHi(0, expected.c_str());
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("event_toobig.wasm");
}

TEST_F(WasmSequencerTester, EventBadSeverityTraps) {
    const Fw::String file = this->copyAsset("event_bad_sev.wasm");

    this->sendCmd_RUN(0, 62, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidSeverity_SIZE(1);
    // The guest requested severity id 99, which is not a valid Fw::LogSeverity.
    ASSERT_EVENTS_HostFunctionInvalidSeverity(0, 99);
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    // The host function returned SPACEWASM_TRAP -> HOST trap reason.
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::HOST);
    this->removeFile("event_bad_sev.wasm");
}

// ----------------------------------------------------------------------
// Host functions: TELEMETRY
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, TelemetryRead) {
    // Canned telemetry payload the getTlmChan handler returns.
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    const Fw::String file = this->copyAsset("tlm.wasm");
    this->sendCmd_RUN(0, 70, file, BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    // getTlmChan was invoked by the guest tlm host call.
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("tlm.wasm");
}

TEST_F(WasmSequencerTester, TelemetryBadTimeSizeTraps) {
    const Fw::String file = this->copyAsset("tlm_badtime.wasm");
    this->sendCmd_RUN(0, 71, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    // A time_size of 8 != Fw::Time::SERIALIZED_SIZE is rejected; the reported
    // maxSize is the required serialized time size.
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::TELEMETRY, 8, Fw::Time::SERIALIZED_SIZE);
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::HOST);
    this->removeFile("tlm_badtime.wasm");
}

TEST_F(WasmSequencerTester, TelemetryTooBigTraps) {
    const Fw::String file = this->copyAsset("tlm_toobig.wasm");
    this->sendCmd_RUN(0, 72, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    // value_size 600 > FW_TLM_BUFFER_MAX_SIZE is rejected.
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::TELEMETRY, 600, FW_TLM_BUFFER_MAX_SIZE);
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::HOST);
    this->removeFile("tlm_toobig.wasm");
}

// ----------------------------------------------------------------------
// Host functions: PARAMETER
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ParameterRead) {
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    const Fw::String file = this->copyAsset("prm.wasm");
    this->sendCmd_RUN(0, 80, file, BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("prm.wasm");
}

TEST_F(WasmSequencerTester, ParameterTooBigTraps) {
    const Fw::String file = this->copyAsset("prm_toobig.wasm");
    this->sendCmd_RUN(0, 81, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    // len 600 > FW_PARAM_BUFFER_MAX_SIZE is rejected.
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::PARAMETER, 600, FW_PARAM_BUFFER_MAX_SIZE);
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::HOST);
    this->removeFile("prm_toobig.wasm");
}

// ----------------------------------------------------------------------
// Host functions: COMMAND (byte-fidelity of guest bytes -> cmdOut ComBuffer)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, CommandByteFidelityAndResume) {
    const Fw::String file = this->copyAsset("cmd.wasm");

    // RUN pauses when the guest calls cmd, dispatches the command out cmdOut,
    // and parks in AWAITING_RESPONSE until we feed a cmdResponseIn.
    this->sendCmd_RUN(0, 90, file, BLOCK);
    this->dispatchUntilState(State::RUNNING_AWAITING_RESPONSE);
    ASSERT_EQ(this->getState(), State::RUNNING_AWAITING_RESPONSE);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::COMMAND);

    // The ComBuffer is a packet descriptor (FW_PACKET_COMMAND) followed by the
    // exact 8 guest bytes.
    ASSERT_from_cmdOut_SIZE(1);
    const Fw::ComBuffer& out = this->fromPortHistory_cmdOut->at(0).data;

    const U8 expectedPattern[8] = {0xde, 0xad, 0xbe, 0xef, 0x01, 0x02, 0x03, 0x04};
    const FwSizeType descSize = static_cast<FwSizeType>(sizeof(FwPacketDescriptorType));
    ASSERT_EQ(out.getSize(), descSize + sizeof expectedPattern);

    const U8* bytes = out.getBuffAddr();
    // Descriptor prefix is FW_PACKET_COMMAND (0), big-endian.
    for (FwSizeType i = 0; i < descSize; i++) {
        ASSERT_EQ(bytes[i], 0);
    }
    // The guest payload round-trips verbatim.
    for (FwSizeType i = 0; i < sizeof expectedPattern; i++) {
        ASSERT_EQ(bytes[descSize + i], expectedPattern[i]) << "payload byte " << i;
    }

    // Feed the command response; the interpreter resumes and finishes.
    this->invoke_to_cmdResponseIn(0, 0, 0, Fw::CmdResponse::OK);
    this->dispatchUntilState(State::READY);
    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, CommandTooBigTraps) {
    const Fw::String file = this->copyAsset("cmd_toobig.wasm");
    this->sendCmd_RUN(0, 91, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    // len 600 + packet descriptor > FW_COM_BUFFER_MAX_SIZE; the reported maxSize
    // is the payload budget after reserving the descriptor prefix.
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::COMMAND, 600,
                                 FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType));
    ASSERT_EVENTS_SequenceTrap_SIZE(1);
    ASSERT_EVENTS_SequenceTrap(0, WasmSequencer_TrapReason::HOST);
    this->removeFile("cmd_toobig.wasm");
}

TEST_F(WasmSequencerTester, UnexpectedCmdResponseFromIdle) {
    // A cmdResponseIn while not awaiting a response is "unexpected". From IDLE
    // the state machine raises stmtResponse_unexpected; it must not crash.
    this->invoke_to_cmdResponseIn(0, 0, 0, Fw::CmdResponse::OK);
    this->dispatchAll();
    ASSERT_EQ(this->getState(), State::IDLE);
}

// ----------------------------------------------------------------------
// Host-function invalid-pointer error paths (guest passes an OOB pointer)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, CommandBadPointerFails) {
    // cmd() with an out-of-bounds buffer pointer: spacewasm_mem_read fails ->
    // HostFunctionInvalidPointer(COMMAND) -> stmtResponse_failure -> IDLE.
    const Fw::String file = this->copyAsset("cmd_badptr.wasm");
    this->sendCmd_RUN(0, 92, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    // The OOB buffer read fails with ERR_MEM_OUT_OF_BOUNDS in the COMMAND path.
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::COMMAND,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 92, Fw::CmdResponse::EXECUTION_ERROR);
    // No command was actually dispatched.
    ASSERT_from_cmdOut_SIZE(0);
    this->removeFile("cmd_badptr.wasm");
}

TEST_F(WasmSequencerTester, EventBadPointerFails) {
    const Fw::String file = this->copyAsset("event_badptr.wasm");
    this->sendCmd_RUN(0, 93, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::EVENT,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    this->removeFile("event_badptr.wasm");
}

TEST_F(WasmSequencerTester, ParameterBadPointerFails) {
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    const Fw::String file = this->copyAsset("prm_badptr.wasm");
    this->sendCmd_RUN(0, 94, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    // getParam was queried, but writing the value into guest memory failed.
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::PARAMETER,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    this->removeFile("prm_badptr.wasm");
}

TEST_F(WasmSequencerTester, TelemetryBadTimePointerFails) {
    // The time write (first mem_write) fails on an OOB time pointer.
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    const Fw::String file = this->copyAsset("tlm_badtimeptr.wasm");
    this->sendCmd_RUN(0, 95, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    // The time (first) mem_write fails on the OOB pointer in the TELEMETRY path.
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::TELEMETRY,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    this->removeFile("tlm_badtimeptr.wasm");
}

TEST_F(WasmSequencerTester, TelemetryBadValuePointerFails) {
    // The time write succeeds but the value write (second mem_write) fails on an
    // OOB value pointer.
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    const Fw::String file = this->copyAsset("tlm_badvalptr.wasm");
    this->sendCmd_RUN(0, 96, file, BLOCK);
    this->dispatchAll();

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    // The value (second) mem_write fails on the OOB pointer in the TELEMETRY path.
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::TELEMETRY,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    this->removeFile("tlm_badvalptr.wasm");
}

// ----------------------------------------------------------------------
// Sleep host functions + checkTimers wake path
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RelativeSleepWakes) {
    const Fw::String file = this->copyAsset("rsleep.wasm");

    // Start at t=0. rsleep asks for a 1s relative timer.
    this->setTestTime(Fw::Time(0, 0));
    this->sendCmd_RUN(0, 100, file, BLOCK);
    this->dispatchUntilState(State::RUNNING_AWAITING_RESPONSE);

    ASSERT_EQ(this->getState(), State::RUNNING_AWAITING_RESPONSE);
    ASSERT_TRUE(this->hasPendingTimer());

    // A checkTimers tick before the deadline does not wake it.
    this->setTestTime(Fw::Time(0, 500000));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    ASSERT_EQ(this->getState(), State::RUNNING_AWAITING_RESPONSE);

    // Past the deadline, the timer fires and the interpreter resumes/finishes.
    this->setTestTime(Fw::Time(2, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilState(State::READY);
    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("rsleep.wasm");
}

TEST_F(WasmSequencerTester, AbsoluteSleepWakes) {
    const Fw::String file = this->copyAsset("asleep.wasm");

    this->setTestTime(Fw::Time(0, 0));
    this->sendCmd_RUN(0, 101, file, BLOCK);
    this->dispatchUntilState(State::RUNNING_AWAITING_RESPONSE);
    ASSERT_TRUE(this->hasPendingTimer());

    // asleep target is 10s from epoch.
    this->setTestTime(Fw::Time(20, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilState(State::READY);
    ASSERT_EQ(this->getState(), State::READY);
    this->removeFile("asleep.wasm");
}

TEST_F(WasmSequencerTester, SleepTimeBaseMismatchFails) {
    const Fw::String file = this->copyAsset("rsleep.wasm");

    // Set the timer using a specific time base.
    this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 0, 0));
    this->sendCmd_RUN(0, 102, file, BLOCK);
    this->dispatchUntilState(State::RUNNING_AWAITING_RESPONSE);
    ASSERT_TRUE(this->hasPendingTimer());

    // Change the time base: comparison becomes INCOMPARABLE -> timeOpFailed.
    this->setTestTime(Fw::Time(TimeBase::TB_SC_TIME, 0, 5, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilState(State::IDLE);

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    this->removeFile("rsleep.wasm");
}

// ----------------------------------------------------------------------
// PAUSE / CONTINUE / CANCEL across the run state machine (loop.wasm)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, PauseThenContinueCompletes) {
    // Tiny fuel makes the busy-loop span many OUT_OF_FUEL cycles so PAUSE can
    // land between statements deterministically.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 110, file, NO_BLOCK);
    // Advance into the running loop.
    this->dispatchUntilState(State::RUNNING_SPINNING);

    // Request a pause; the machine pauses before the next spin.
    this->sendCmd_PAUSE(0, 111);
    this->dispatchUntilState(State::RUNNING_PAUSED);
    ASSERT_EQ(this->getState(), State::RUNNING_PAUSED);
    ASSERT_EVENTS_SequenceBroken_SIZE(1);

    // Continue: it resumes spinning and eventually finishes.
    this->sendCmd_CONTINUE(0, 112);
    this->dispatchUntilState(State::READY);
    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, CancelWhileSpinning) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 120, file, NO_BLOCK);
    this->dispatchUntilState(State::RUNNING_SPINNING);

    // CANCEL is synchronous (responds OK immediately) and returns to IDLE.
    this->sendCmd_CANCEL(0, 121);
    this->dispatchUntilState(State::IDLE);

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, CancelWhilePaused) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 122, file, NO_BLOCK);
    this->dispatchUntilState(State::RUNNING_SPINNING);

    this->sendCmd_PAUSE(0, 123);
    this->dispatchUntilState(State::RUNNING_PAUSED);

    this->sendCmd_CANCEL(0, 124);
    this->dispatchUntilState(State::IDLE);
    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, CancelWhileAwaitingResponse) {
    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 125, file, BLOCK);
    this->dispatchUntilState(State::RUNNING_AWAITING_RESPONSE);

    // CANCEL from AWAITING_RESPONSE clears the pending host function and returns
    // to IDLE.
    this->sendCmd_CANCEL(0, 126);
    this->dispatchUntilState(State::IDLE);
    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::NONE);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, CancelFromReadyReturnsIdle) {
    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_LOAD(0, 130, file);
    this->dispatchUntilState(State::READY);

    this->sendCmd_CANCEL(0, 131);
    this->dispatchUntilState(State::IDLE);
    ASSERT_EQ(this->getState(), State::IDLE);
    this->removeFile("empty.wasm");
}

// ----------------------------------------------------------------------
// Commands rejected / queued while a sequence is running
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RunWhileRunningRejected) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 150, file, NO_BLOCK);
    this->dispatchUntilState(State::RUNNING_SPINNING);

    // RUN is only valid from IDLE/READY; from RUNNING it is rejected
    // (RUN_cmdHandler invalid-state guard) without disturbing the running loop.
    // The original NO_BLOCK RUN already responded OK at load (index 0); the
    // rejected RUN lands as an EXECUTION_ERROR (index 1). The loop still finishes.
    this->sendCmd_RUN(0, 151, file, NO_BLOCK);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_InvalidCommand_SIZE(1);
    ASSERT_EVENTS_InvalidCommand(0, WasmSequencer_SequencerStateMachine_State::RUNNING_SPINNING);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 150, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_RUN, 151, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, LoadWhileRunningRejected) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 152, file, NO_BLOCK);
    this->dispatchUntilState(State::RUNNING_SPINNING);

    // LOAD_NAME invalid-state guard (only IDLE/READY accepted). The original
    // NO_BLOCK RUN responded OK at load (index 0); the rejected LOAD is
    // EXECUTION_ERROR (index 1). The loop still finishes.
    this->sendCmd_LOAD(0, 153, file);
    this->dispatchUntilState(State::READY);

    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_InvalidCommand_SIZE(1);
    ASSERT_EVENTS_InvalidCommand(0, WasmSequencer_SequencerStateMachine_State::RUNNING_SPINNING);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 152, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_LOAD, 153, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, WaitWhileRunningQueuesUntilFinish) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 160, file, NO_BLOCK);
    this->dispatchUntilState(State::RUNNING_SPINNING);

    // WAIT while running enqueues on the finish queue (default branch of
    // WAIT_cmdHandler) and only responds once the loop completes.
    this->sendCmd_WAIT(0, 161);
    this->dispatchUntilState(State::READY);

    // The RUN already responded OK at load (NO_BLOCK); the WAIT responds OK on
    // finish. Both land in the cmd-response history.
    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 160, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_WAIT, 161, Fw::CmdResponse::OK);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, UnexpectedCmdResponseWhileSpinningFails) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 170, file, NO_BLOCK);
    this->dispatchUntilState(State::RUNNING_SPINNING);

    // A cmdResponseIn while spinning (not awaiting a host command) is "unexpected"
    // and fails the running sequence (stmtResponse_unexpected -> report_seqFailed).
    this->invoke_to_cmdResponseIn(0, 0, 0, Fw::CmdResponse::OK);
    this->dispatchUntilState(State::IDLE);

    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, WaitFinishQueueOverflow) {
    // The blocking-finish queue (m_pendingFinishCmds) is 8 deep. Fill it with
    // WAITs while a sequence is running; the 9th overflows and is rejected with
    // TooManyBlockingCommands + EXECUTION_ERROR (WAIT_cmdHandler enqueue-failure).
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 180, file, NO_BLOCK);
    this->dispatchUntilState(State::RUNNING_SPINNING);

    // Queue nine WAITs up front (test queue depth is 20) so they are all handled
    // while the loop is still spinning: eight enqueue, the ninth overflows.
    for (U32 i = 0; i < 9; i++) {
        this->sendCmd_WAIT(0, 181 + i);
    }
    this->dispatchUntilState(State::READY);

    // All queued WAITs are drained on finish and the sequence returns to READY.
    ASSERT_EQ(this->getState(), State::READY);
    ASSERT_EVENTS_TooManyBlockingCommands_SIZE(1);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, WriteTelemetryTickIsNoOp) {
    // The writeTelemetry port handler is an intentional no-op; dispatching a tick
    // must not change state or emit anything.
    this->invoke_to_writeTelemetry(0, 0);
    this->dispatchAll();
    ASSERT_EQ(this->getState(), State::IDLE);
    ASSERT_CMD_RESPONSE_SIZE(0);
    ASSERT_EVENTS_SIZE(0);
}

// ----------------------------------------------------------------------
// Continue is a no-op while already running
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ContinueWhileSpinningIsOk) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 140, file, NO_BLOCK);
    this->dispatchUntilState(State::RUNNING_SPINNING);

    this->sendCmd_CONTINUE(0, 141);
    this->dispatchUntilState(State::READY);
    // CONTINUE while running just responds OK; the loop still finishes.
    ASSERT_EQ(this->getState(), State::READY);
    this->removeFile("loop.wasm");
}

}  // namespace Svc

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
