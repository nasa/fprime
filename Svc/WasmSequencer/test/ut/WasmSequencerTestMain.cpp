// ======================================================================
// \title  WasmSequencerTestMain.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component test main function
// ======================================================================

#include "WasmSequencerGTestBase.hpp"
#include "WasmSequencerTester.hpp"

#include "WasmSequencer_ControllerStateMachine_StateEnumAc.hpp"
#include "config/FwPacketDescriptorTypeAliasAc.h"

namespace Svc {

using ControllerState = WasmSequencerTester::ControllerState;
using EngineState = WasmSequencerTester::EngineState;

// Convenience: the two block modes.
static const Svc::BlockState BLOCK(Svc::BlockState::BLOCK);
static const Svc::BlockState NO_BLOCK(Svc::BlockState::NO_BLOCK);

// ----------------------------------------------------------------------
// Construction / initial state
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, InitialStateIsIdle) {
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // No pending work on a fresh component.
    ASSERT_FALSE(this->hasPendingLoadCmd());
    ASSERT_FALSE(this->hasPendingTimer());
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// LOAD / LOAD_NAME (nominal + failures)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, LoadEmptyModuleReady) {
    const Fw::String file = this->copyAsset("empty.wasm");

    this->sendCmd_LOAD(0, 10, file);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 10, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, LoadResolvesAgainstSeqBaseDir) {
    // With SEQ_BASE_DIR set, the requested (bare) file name is resolved relative
    // to the base dir with a single '/' separator. copyAsset stages the module in
    // the CWD, so a base dir of "." resolves "empty.wasm" -> "./empty.wasm", which
    // opens successfully.
    this->paramSet_SEQ_BASE_DIR(Fw::ParamString("."), Fw::ParamValid::VALID);
    this->component.loadParameters();

    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_LOAD(0, 11, file);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 11, Fw::CmdResponse::OK);
    ASSERT_EVENTS_FileOpenError_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, LoadAgainstMissingBaseDirFailsToOpen) {
    // A non-empty base dir that does not contain the file resolves to a path that
    // cannot be opened; the load fails with FileOpenError and returns to IDLE.
    this->paramSet_SEQ_BASE_DIR(Fw::ParamString("no_such_dir"), Fw::ParamValid::VALID);
    this->component.loadParameters();

    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_LOAD(0, 12, file);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_FileOpenError_SIZE(1);
    // The path that failed to open is the base dir joined with the file name.
    ASSERT_STREQ(this->eventHistory_FileOpenError->at(0).fileName.toChar(), "no_such_dir/empty.wasm");
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 12, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, LoadNamedModuleReady) {
    const Fw::String file = this->copyAsset("empty.wasm");

    this->sendCmd_LOAD_NAME(0, 11, file, Fw::CmdStringArg("mod"));
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD_NAME, 11, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, LoadStartModuleRespondsOk) {
    // A LOAD whose module carries a (running) Wasm start function drives
    // STARTING -> startInvoked -> RUNNING and spins the start to completion. The
    // load command must be answered when the start finishes and we settle in
    // READY -- not left dangling (which previously also wedged the single load-cmd
    // slot, tripping an assert on the next load).
    const Fw::String file = this->copyAsset("start.wasm");

    this->sendCmd_LOAD(0, 40, file);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 40, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("start.wasm");
}

TEST_F(WasmSequencerTester, LoadStartModuleTrapRespondsError) {
    // The module's start function begins running then traps (unreachable). The
    // pending load command must receive EXECUTION_ERROR as we fall back to IDLE.
    const Fw::String file = this->copyAsset("start_trap.wasm");

    this->sendCmd_LOAD(0, 41, file);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0,
                                 WasmSequencer_TrapReason::UNREACHABLE, WasmSequencer_HostFunction::NONE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 41, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("start_trap.wasm");
}

TEST_F(WasmSequencerTester, LoadStartModuleTwiceDoesNotWedge) {
    // Regression: the load-cmd slot must be freed on the startInvoked->RUNNING
    // completion path. A second LOAD-with-start would otherwise trip the
    // FW_ASSERT(!m_hasPendingLoadCmd) guard in LOAD_NAME_cmdHandler.
    const Fw::String file = this->copyAsset("start.wasm");

    this->sendCmd_LOAD(0, 42, file);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 42, Fw::CmdResponse::OK);

    // Second load from READY reuses the store and runs the start again. The
    // component is already in READY, so drain the queue rather than waiting for a
    // state change that has effectively already happened.
    this->sendCmd_LOAD(0, 43, file);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, OPCODE_LOAD, 43, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("start.wasm");
}

TEST_F(WasmSequencerTester, LoadFileNotFound) {
    // No copyAsset: the file simply does not exist in the CWD.
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("does_not_exist.wasm");
    this->sendCmd_LOAD(0, 12, Fw::CmdStringArg("does_not_exist.wasm"));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
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

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // A bad wasm magic number decodes to ERR_MALFORMED_MAGIC.
    ASSERT_EVENTS_ModuleLoadFailed(0, WasmSequencer_Status::ERR_MALFORMED_MAGIC);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 13, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("malformed.wasm");
}

TEST_F(WasmSequencerTester, LoadTruncated) {
    const Fw::String file = this->copyAsset("truncated.wasm");
    this->sendCmd_LOAD(0, 14, file);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("truncated.wasm");
}

TEST_F(WasmSequencerTester, LoadBigMemFailsAllocator) {
    // Requests more guest pages than the 2048-byte guest pool can back.
    const Fw::String file = this->copyAsset("bigmem.wasm");
    this->sendCmd_LOAD(0, 15, file);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // A failed load must return the guest allocator to empty (no fragmentation).
    ASSERT_EQ(this->getGuestOffset(), static_cast<FwSizeType>(0));
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("bigmem.wasm");
}

TEST_F(WasmSequencerTester, LoadBadImportNameFails) {
    const Fw::String file = this->copyAsset("bad_import_name.wasm");
    this->sendCmd_LOAD(0, 16, file);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("bad_import_name.wasm");
}

TEST_F(WasmSequencerTester, LoadBadImportModuleFails) {
    const Fw::String file = this->copyAsset("bad_import_module.wasm");
    this->sendCmd_LOAD(0, 17, file);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("bad_import_module.wasm");
}

TEST_F(WasmSequencerTester, LoadBadImportSigFails) {
    const Fw::String file = this->copyAsset("bad_import_sig.wasm");
    this->sendCmd_LOAD(0, 18, file);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("bad_import_sig.wasm");
}

// ----------------------------------------------------------------------
// RUN (nominal completion, start functions, traps)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RunEmptyNoBlock) {
    const Fw::String file = this->copyAsset("empty.wasm");

    // NO_BLOCK responds OK once the module loads; the program then runs to
    // completion and lands in READY.
    this->sendCmd_RUN(0, 20, file, NO_BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 20, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, RunEmptyBlock) {
    const Fw::String file = this->copyAsset("empty.wasm");

    // BLOCK holds the command response until the interpreter finishes.
    this->sendCmd_RUN(0, 21, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 21, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, RunStartModule) {
    // Module with a `start` function that runs at instantiation.
    const Fw::String file = this->copyAsset("start.wasm");

    this->sendCmd_RUN(0, 22, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 22, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("start.wasm");
}

TEST_F(WasmSequencerTester, RunNoMainFailsInvoke) {
    // Valid module that exports `other` but not `main`.
    const Fw::String file = this->copyAsset("no_main.wasm");

    this->sendCmd_RUN(0, 23, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_InvalidModuleEntrypoint_SIZE(1);
    // No `main` export resolves to ERR_NOT_FOUND.
    ASSERT_EVENTS_InvalidModuleEntrypoint(0, 0, WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 23, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("no_main.wasm");
}

TEST_F(WasmSequencerTester, RunMainInvokeFails) {
    // A module whose `main` declares more locals than fit the guest stack. main
    // has a valid [] -> [] signature so moduleHasValidMain passes, but
    // spacewasm_invoke fails at call setup (StackOverflow), taking the
    // MAIN_INVOKE_CHECK failure branch: reportModuleMainInvokeFailed +
    // EXECUTION_ERROR responses -> IDLE. This is the main-invoke analogue of
    // RunStartOverflowTrapsToIdle.
    const Fw::String file = this->copyAsset("main_overflow.wasm");

    this->sendCmd_RUN(0, 40, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleMainInvokeFailed_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 40, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("main_overflow.wasm");
}

TEST_F(WasmSequencerTester, RunMainReturningI32Succeeds) {
    // main has the [] -> i32 signature (the other form validateModuleMain
    // accepts). It returns a non-zero value; a normal interpreter finish is a
    // success regardless of the returned i32 (only exit(code!=0)/panic/trap fail
    // a sequence), so the run succeeds and lands READY.
    const Fw::String file = this->copyAsset("main_i32.wasm");

    this->sendCmd_RUN(0, 42, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 42, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("main_i32.wasm");
}

TEST_F(WasmSequencerTester, RunUnreachableTraps) {
    const Fw::String file = this->copyAsset("unreachable.wasm");

    this->sendCmd_RUN(0, 24, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0,
                                 WasmSequencer_TrapReason::UNREACHABLE, WasmSequencer_HostFunction::NONE);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 24, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("unreachable.wasm");
}

TEST_F(WasmSequencerTester, RunDivZeroTraps) {
    const Fw::String file = this->copyAsset("divzero.wasm");

    this->sendCmd_RUN(0, 25, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0,
                                 WasmSequencer_TrapReason::DIVIDE_BY_ZERO, WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("divzero.wasm");
}

TEST_F(WasmSequencerTester, RunExitNonZeroFails) {
    // exit.wasm calls fprime_v1.exit(1). A non-zero exit is a program failure,
    // surfaced as a ProgramExited event (not a trap) with an EXECUTION_ERROR.
    const Fw::String file = this->copyAsset("exit.wasm");

    this->sendCmd_RUN(0, 26, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::HOST_EXIT, 1, WasmSequencer_TrapReason::NONE,
                                 WasmSequencer_HostFunction::NONE);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 26, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("exit.wasm");
}

TEST_F(WasmSequencerTester, RunPanicFails) {
    // panic.wasm calls fprime_v1.panic(7). A panic is a program failure, surfaced
    // as a PanicOccurred event (not a trap).
    const Fw::String file = this->copyAsset("panic.wasm");

    this->sendCmd_RUN(0, 27, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::HOST_PANIC, 7, WasmSequencer_TrapReason::NONE,
                                 WasmSequencer_HostFunction::NONE);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 27, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("panic.wasm");
}

TEST_F(WasmSequencerTester, RunExitZeroSucceeds) {
    // exit0.wasm calls fprime_v1.exit(0). A zero exit code is a clean success:
    // no trap, no ProgramExited event, and an OK response.
    const Fw::String file = this->copyAsset("exit0.wasm");

    this->sendCmd_RUN(0, 29, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 29, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("exit0.wasm");
}

TEST_F(WasmSequencerTester, RunStartTrapsToIdle) {
    // A module whose `start` function contains `unreachable`. The interpreter
    // begins the start function (startInvoked -> RUNNING) and traps while
    // spinning, surfacing as a SequenceTrap and returning to IDLE with an
    // EXECUTION_ERROR response.
    const Fw::String file = this->copyAsset("start_trap.wasm");

    this->sendCmd_RUN(0, 28, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0,
                                 WasmSequencer_TrapReason::UNREACHABLE, WasmSequencer_HostFunction::NONE);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 28, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("start_trap.wasm");
}

TEST_F(WasmSequencerTester, RunStartOverflowTrapsToIdle) {
    // A module whose `start` function declares more locals than fit the guest
    // stack. spacewasm_invoke_start fails at call setup (StackOverflow) and
    // returns SPACEWASM_RUN_TRAP *directly* -- exercising the startError branch
    // (STARTING -> invokeStartOfLastModule -> startError -> reportInvokeFailure).
    // This is distinct from start_trap.wasm, whose start begins running
    // (RUN_OUT_OF_FUEL -> startInvoked -> RUNNING) and only traps while spinning.
    const Fw::String file = this->copyAsset("start_overflow.wasm");

    this->sendCmd_RUN(0, 29, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // reportModuleStartInvokeFailed fires; the exact status is not asserted
    // because m_invokeStatus is not set on the start-invoke path.
    ASSERT_EVENTS_ModuleStartInvokeFailed_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 29, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("start_overflow.wasm");
}

// NOTE: the startPause branch (a start function that calls a pausing host
// function -> reportUnsupportedStartPause) is intentionally not covered here.
// It is unreachable with the current host set: start functions are validated as
// []->[], and every pausing fprime_v1 host function takes parameters, so no
// []->[] pausing host start function can be constructed.

// ----------------------------------------------------------------------
// INVOKE
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, InvokeAfterLoad) {
    const Fw::String file = this->copyAsset("empty.wasm");

    // Load (unnamed) then invoke "main" of the unnamed module.
    this->sendCmd_LOAD(0, 30, file);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);

    // Already READY, so pump the queue to completion rather than waiting on a
    // state change (INVOKE of a trivial main returns straight back to READY).
    this->sendCmd_INVOKE(0, 31, Fw::CmdStringArg(""), BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 31, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, InvokeTrapAfterExitZeroIsNotMisreported) {
    // Regression: exit(0) returns the component to READY without resetting the
    // store, leaving the host exit disposition (HOST_EXIT/0) stale. A subsequent
    // invoke that genuinely traps must reset that disposition per-invoke so the
    // trap is reported as a trap, not misread as a clean completion.
    const Fw::String file = this->copyAsset("exit0_then_trap.wasm");

    this->sendCmd_LOAD(0, 37, file);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);

    // First invoke: exit(0) -> clean success -> back to READY (store not reset).
    this->sendCmd_INVOKE(0, 38, Fw::CmdStringArg(""), BLOCK, {});
    this->dispatchAll();
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 38, Fw::CmdResponse::OK);

    // Second invoke on the same store: unreachable -> genuine trap. Without the
    // per-invoke reset this would surface as another SequenceSucceeded + OK.
    this->sendCmd_INVOKE(0, 39, Fw::CmdStringArg(""), BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0,
                                 WasmSequencer_TrapReason::UNREACHABLE, WasmSequencer_HostFunction::NONE);
    // Still exactly one success (from the first invoke); the trap did not add one.
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(2, OPCODE_INVOKE, 39, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("exit0_then_trap.wasm");
}

TEST_F(WasmSequencerTester, InvokeNoBlockRespondsImmediately) {
    const Fw::String file = this->copyAsset("empty.wasm");

    this->sendCmd_LOAD(0, 35, file);
    this->dispatchUntilControllerState(ControllerState::READY);

    // NO_BLOCK INVOKE responds OK as soon as the module resolves, before the
    // function actually runs (INVOKE_cmdHandler NO_BLOCK path).
    this->sendCmd_INVOKE(0, 36, Fw::CmdStringArg(""), NO_BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 36, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, InvokeUnknownModule) {
    const Fw::String file = this->copyAsset("empty.wasm");

    this->sendCmd_LOAD(0, 32, file);
    this->dispatchUntilControllerState(ControllerState::READY);

    // A module name that was never loaded resolves to not-found.
    this->sendCmd_INVOKE(0, 33, Fw::CmdStringArg("nope"), NO_BLOCK, {});
    this->dispatchAll();

    ASSERT_EVENTS_ModuleNotFound_SIZE(1);
    ASSERT_EVENTS_ModuleNotFound(0, "nope");
    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 33, Fw::CmdResponse::EXECUTION_ERROR);
    // State is unchanged (still READY).
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, InvokeFromIdleInvalid) {
    // INVOKE is only valid from READY.
    this->sendCmd_INVOKE(0, 34, Fw::CmdStringArg(""), NO_BLOCK, {});
    this->dispatchAll();

    ASSERT_EVENTS_InvalidCommand_SIZE(1);
    ASSERT_EVENTS_InvalidCommand(0, WasmSequencer_ControllerStateMachine_State::IDLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_INVOKE, 34, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// WAIT
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, WaitFromIdleRespondsImmediately) {
    this->sendCmd_WAIT(0, 40);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(0, OPCODE_WAIT, 40, Fw::CmdResponse::OK);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, WaitFromReadyRespondsImmediately) {
    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_LOAD(0, 41, file);
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_WAIT(0, 42);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(1, OPCODE_WAIT, 42, Fw::CmdResponse::OK);
    // WAIT from READY responds immediately without changing state.
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

// ----------------------------------------------------------------------
// Invalid-state command rejections
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ContinueFromIdleInvalid) {
    this->sendCmd_CONTINUE(0, 50);
    this->dispatchAll();
    ASSERT_EVENTS_InvalidCommand_SIZE(1);
    ASSERT_EVENTS_InvalidCommand(0, WasmSequencer_ControllerStateMachine_State::IDLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_CONTINUE, 50, Fw::CmdResponse::EXECUTION_ERROR);
    // The rejected command leaves the component in IDLE.
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
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
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// Host functions: EVENT
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, EventActivityHi) {
    const Fw::String file = this->copyAsset("event.wasm");

    this->sendCmd_RUN(0, 60, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_LogActivityHi_SIZE(1);
    ASSERT_EVENTS_LogActivityHi(0, "hello wasm");
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("event.wasm");
}

TEST_F(WasmSequencerTester, EventAllSeverities) {
    const Fw::String file = this->copyAsset("event_all_sev.wasm");

    this->sendCmd_RUN(0, 61, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    // The fixture emits severities 1..7. FATAL (1) and COMMAND (4) are forbidden
    // for guest programs, so each is reported via HostFunctionInvalidSeverity
    // (carrying the raw id and the guest message) instead of a Log* event. The
    // remaining five map to their matching guest-severity events. Every emitted
    // event carries the fixture's "sev" message, so assert the payload too.
    ASSERT_EVENTS_LogWarningHi_SIZE(1);
    ASSERT_EVENTS_LogWarningHi(0, "sev");
    ASSERT_EVENTS_LogWarningLo_SIZE(1);
    ASSERT_EVENTS_LogWarningLo(0, "sev");
    ASSERT_EVENTS_LogActivityHi_SIZE(1);
    ASSERT_EVENTS_LogActivityHi(0, "sev");
    ASSERT_EVENTS_LogActivityLo_SIZE(1);
    ASSERT_EVENTS_LogActivityLo(0, "sev");
    ASSERT_EVENTS_LogDiagnostic_SIZE(1);
    ASSERT_EVENTS_LogDiagnostic(0, "sev");
    // FATAL (1) and COMMAND (4) rejected, message preserved.
    ASSERT_EVENTS_HostFunctionInvalidSeverity_SIZE(2);
    ASSERT_EVENTS_HostFunctionInvalidSeverity(0, Fw::LogSeverity::FATAL, "sev");
    ASSERT_EVENTS_HostFunctionInvalidSeverity(1, Fw::LogSeverity::COMMAND, "sev");
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("event_all_sev.wasm");
}

TEST_F(WasmSequencerTester, EventMessageTruncatedToMax) {
    // The guest passes a 250-byte message length; the host clamps it to
    // FW_LOG_STRING_MAX_SIZE (200) before reading (wasmEvent len clamp). The
    // emitted event string is exactly 200 'A' characters.
    const Fw::String file = this->copyAsset("event_toobig.wasm");

    this->sendCmd_RUN(0, 63, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_LogActivityHi_SIZE(1);

    // The host reads at most FW_LOG_STRING_MAX_SIZE (200) bytes; the LogActivityHi
    // event string arg further caps at its declared `string size 128`. So the
    // observed message is 128 'A' characters -- all identical, so truncation at
    // either bound yields the same run of 'A's, confirming the clamp fired.
    const std::string expected(128, 'A');
    ASSERT_EVENTS_LogActivityHi(0, expected.c_str());
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("event_toobig.wasm");
}

TEST_F(WasmSequencerTester, EventBadSeverityReported) {
    const Fw::String file = this->copyAsset("event_bad_sev.wasm");

    this->sendCmd_RUN(0, 62, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    // The guest requested severity id 99, which is not a valid Fw::LogSeverity.
    // Rather than trap, the host reports it (with the guest message) and lets the
    // guest continue, so the sequence runs to completion.
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_HostFunctionInvalidSeverity_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidSeverity(0, 99, "x");
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("event_bad_sev.wasm");
}

// ----------------------------------------------------------------------
// Host functions: TELEMETRY
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, TelemetryRead) {
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    const Fw::String file = this->copyAsset("tlm.wasm");
    this->sendCmd_RUN(0, 70, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("tlm.wasm");
}

TEST_F(WasmSequencerTester, TelemetryReadValueMismatchTraps) {
    U8 raw[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    const Fw::String file = this->copyAsset("tlm.wasm");
    this->sendCmd_RUN(0, 73, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0,
                                 WasmSequencer_TrapReason::UNREACHABLE, WasmSequencer_HostFunction::TELEMETRY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("tlm.wasm");
}

TEST_F(WasmSequencerTester, TelemetryBadTimeSizeTraps) {
    const Fw::String file = this->copyAsset("tlm_badtime.wasm");
    this->sendCmd_RUN(0, 71, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // A time_size of 8 < Fw::Time::SERIALIZED_SIZE cannot hold the serialized time and
    // is rejected as too small; the reported valueSize is the required serialized size.
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::TELEMETRY, 8, Fw::Time::SERIALIZED_SIZE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0, WasmSequencer_TrapReason::HOST,
                                 WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("tlm_badtime.wasm");
}

TEST_F(WasmSequencerTester, TelemetryBadTimeSizeTooLargeTraps) {
    const Fw::String file = this->copyAsset("tlm_bigtime.wasm");
    this->sendCmd_RUN(0, 74, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // A time_size of 16 > Fw::Time::SERIALIZED_SIZE must match exactly and is
    // rejected as too large; the reported maxSize is the required serialized size.
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::TELEMETRY, 16, Fw::Time::SERIALIZED_SIZE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0, WasmSequencer_TrapReason::HOST,
                                 WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("tlm_bigtime.wasm");
}

TEST_F(WasmSequencerTester, TelemetryOversizedRequestWritesOnlyValueBytes) {
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    const Fw::String file = this->copyAsset("tlm_largebuf.wasm");
    this->sendCmd_RUN(0, 72, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_EVENTS_BufferTooLarge_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("tlm_largebuf.wasm");
}

TEST_F(WasmSequencerTester, TelemetryUndersizedRequestFails) {
    // A value_size (2) smaller than the actual serialized value (4) is rejected at
    // dispatch with BufferTooSmall; nothing is written into guest memory.
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    const Fw::String file = this->copyAsset("tlm_toosmall.wasm");
    this->sendCmd_RUN(0, 74, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    // value_size 2 smaller than the 4-byte serialized value.
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::TELEMETRY, 2, static_cast<U32>(sizeof raw));
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("tlm_toosmall.wasm");
}

// ----------------------------------------------------------------------
// Host functions: PARAMETER
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ParameterRead) {
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    const Fw::String file = this->copyAsset("prm.wasm");
    this->sendCmd_RUN(0, 80, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("prm.wasm");
}

TEST_F(WasmSequencerTester, ParameterReadValueMismatchTraps) {
    U8 raw[4] = {0x00, 0x11, 0x22, 0x33};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    const Fw::String file = this->copyAsset("prm.wasm");
    this->sendCmd_RUN(0, 82, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0,
                                 WasmSequencer_TrapReason::UNREACHABLE, WasmSequencer_HostFunction::PARAMETER);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("prm.wasm");
}

TEST_F(WasmSequencerTester, ParameterOversizedRequestWritesOnlyValueBytes) {
    // Mirror of TelemetryOversizedRequestWritesOnlyValueBytes: an oversized len (64)
    // for a 4-byte value writes only the real value bytes. prm_toobig poisons the byte
    // past the value and traps unless it survives, so success proves no stack leak.
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    const Fw::String file = this->copyAsset("prm_largebuf.wasm");
    this->sendCmd_RUN(0, 81, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_EVENTS_BufferTooLarge_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("prm_largebuf.wasm");
}

TEST_F(WasmSequencerTester, ParameterUndersizedRequestFails) {
    // A len (2) smaller than the actual serialized value (4) is rejected at dispatch
    // with BufferTooSmall; nothing is written into guest memory.
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    const Fw::String file = this->copyAsset("prm_toosmall.wasm");
    this->sendCmd_RUN(0, 83, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::PARAMETER, 2, static_cast<U32>(sizeof raw));
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("prm_toosmall.wasm");
}

// ----------------------------------------------------------------------
// Host functions: ARGS (sequence arguments host->guest round trip)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ArgsRoundTrip) {
    // The RUN command carries the sequence arguments; the guest args() host call reads
    // them back into linear memory. args.wasm requests a 64-byte buffer, verifies the
    // returned count is 4, that the injected pattern round-trips verbatim, and that the
    // byte past the args is untouched (no host stack leak). A clean SequenceSucceeded is
    // a genuine host->guest argument round trip.
    const U8 argBytes[4] = {0xCA, 0xFE, 0xBA, 0xBE};
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    const Fw::String file = this->copyAsset("args.wasm");
    this->sendCmd_RUN(0, 200, file, BLOCK, args);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("args.wasm");
}

TEST_F(WasmSequencerTester, ArgsRoundTripMismatchTraps) {
    // Negative control: inject a different pattern than args.wasm hard-codes. The guest
    // reads the args back, sees the mismatch, and traps (UNREACHABLE). This proves the
    // round-trip check in ArgsRoundTrip is real: if the host dropped the args, this test
    // would (wrongly) succeed too.
    const U8 argBytes[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    const Fw::String file = this->copyAsset("args.wasm");
    this->sendCmd_RUN(0, 201, file, BLOCK, args);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0,
                                 WasmSequencer_TrapReason::UNREACHABLE, WasmSequencer_HostFunction::ARGS);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("args.wasm");
}

TEST_F(WasmSequencerTester, ArgsEmpty) {
    // No arguments supplied (default-constructed SeqArgs, size 0). args_empty.wasm
    // asserts the returned count is 0 and that nothing was written to guest memory.
    const Fw::String file = this->copyAsset("args_empty.wasm");
    this->sendCmd_RUN(0, 202, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("args_empty.wasm");
}

TEST_F(WasmSequencerTester, ArgsUndersizedBufferFails) {
    // The guest declares a 2-byte buffer but 4 arg bytes are present. Writing them would
    // overrun the guest's intent, so the host rejects it at dispatch with
    // BufferTooSmall(ARGS, 2, 4) -> SequenceFailed, writing nothing to guest memory.
    const U8 argBytes[4] = {0xCA, 0xFE, 0xBA, 0xBE};
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    const Fw::String file = this->copyAsset("args_toosmall.wasm");
    this->sendCmd_RUN(0, 203, file, BLOCK, args);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::ARGS, 2, static_cast<U32>(sizeof argBytes));
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("args_toosmall.wasm");
}

TEST_F(WasmSequencerTester, ArgsBadPointerFails) {
    // The guest declares an ample buffer (passes the too-small guard) but points args()
    // at an out-of-bounds address. The mem_write fails ->
    // HostFunctionInvalidPointer(ARGS) -> SequenceFailed.
    const U8 argBytes[4] = {0xCA, 0xFE, 0xBA, 0xBE};
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    const Fw::String file = this->copyAsset("args_badptr.wasm");
    this->sendCmd_RUN(0, 204, file, BLOCK, args);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::ARGS,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("args_badptr.wasm");
}

// ----------------------------------------------------------------------
// Time host function (fprime.time)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, TimeRead) {
    // The guest calls time(0, SERIALIZED_SIZE) into a valid buffer. The host
    // getTime()s the injected test time, serializes it into guest memory, and
    // resumes. A clean SequenceSucceeded proves the TIME host function round trip.
    this->setTestTime(Fw::Time(1000, 2000));

    const Fw::String file = this->copyAsset("time.wasm");
    this->sendCmd_RUN(0, 210, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceFailed_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("time.wasm");
}

TEST_F(WasmSequencerTester, TimeBadPointerFails) {
    // The guest requests a valid time_size but points time() at an out-of-bounds
    // address. The size guards pass; the service block's mem_write then fails ->
    // HostFunctionInvalidPointer(TIME) -> SequenceFailed.
    const Fw::String file = this->copyAsset("time_badptr.wasm");
    this->sendCmd_RUN(0, 211, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::TIME,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("time_badptr.wasm");
}

TEST_F(WasmSequencerTester, TimeBadSizeTraps) {
    // The guest requests a time_size of 8 < Fw::Time::SERIALIZED_SIZE (11), which
    // cannot hold the serialized time. wasmTime rejects it up front with
    // BufferTooSmall(TIME, 8, 11) -> TRAP (HOST trap reason); nothing is written.
    const Fw::String file = this->copyAsset("time_toosmall.wasm");
    this->sendCmd_RUN(0, 212, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::TIME, 8, Fw::Time::SERIALIZED_SIZE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0, WasmSequencer_TrapReason::HOST,
                                 WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("time_toosmall.wasm");
}

TEST_F(WasmSequencerTester, TimeBadSizeTooLargeTraps) {
    // The guest requests a time_size of 16 > Fw::Time::SERIALIZED_SIZE (11), which
    // must match exactly. wasmTime rejects it up front with
    // BufferTooLarge(TIME, 16, 11) -> TRAP (HOST trap reason); nothing is written.
    const Fw::String file = this->copyAsset("time_toobig.wasm");
    this->sendCmd_RUN(0, 213, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::TIME, 16, Fw::Time::SERIALIZED_SIZE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0, WasmSequencer_TrapReason::HOST,
                                 WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("time_toobig.wasm");
}

// ----------------------------------------------------------------------
// Telemetry channels (populated by the counters, flushed via writeTelemetry)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, TelemetryInitialDefaults) {
    // A fresh component reports zeroed counters, IDLE state, no trap, empty name.
    this->flushTelemetry();

    ASSERT_TLM_ControllerState(0, ControllerState::IDLE);
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(0));
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(0));
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(0));
    ASSERT_TLM_CommandsDispatched(0, static_cast<U64>(0));
    ASSERT_TLM_CommandsFailed(0, static_cast<U64>(0));
    ASSERT_TLM_LastTrapReason(0, WasmSequencer_TrapReason::NONE);
    ASSERT_TLM_SeqName(0, "");
}

TEST_F(WasmSequencerTester, TelemetrySuccessCountAndName) {
    // A completed RUN increments SequencesSucceeded, leaves the component READY, and
    // records the sequence name as the filename stem (empty.wasm -> "empty").
    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_RUN(0, 300, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_ControllerState(0, ControllerState::READY);
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(0));
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(0));
    ASSERT_TLM_LastTrapReason(0, WasmSequencer_TrapReason::NONE);
    ASSERT_TLM_SeqName(0, "empty");
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, TelemetrySuccessCountAccumulates) {
    // Counters are cumulative across sequences: two successful runs -> 2.
    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_RUN(0, 301, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);

    // The second RUN starts from READY, so drain the queue rather than waiting for a
    // state change that has effectively already happened.
    this->sendCmd_RUN(0, 302, file, BLOCK, {});
    this->dispatchAll();
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(2);

    this->flushTelemetry();

    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(2));
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(0));
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, TelemetryLoadNameRecordsModuleName) {
    // LOAD_NAME records the user-provided module name verbatim (not the filename).
    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_LOAD_NAME(0, 303, file, Fw::CmdStringArg("mod"));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_SeqName(0, "mod");
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, TelemetryFailedCount) {
    // A no-main module fails to invoke -> SequencesFailed increments, others stay 0.
    const Fw::String file = this->copyAsset("no_main.wasm");
    this->sendCmd_RUN(0, 304, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(0));
    ASSERT_TLM_LastTrapReason(0, WasmSequencer_TrapReason::NONE);
    this->removeFile("no_main.wasm");
}

TEST_F(WasmSequencerTester, TelemetryTrapRecordsReasonAndFails) {
    // A trap records LastTrapReason and counts as a failed sequence.
    const Fw::String file = this->copyAsset("unreachable.wasm");
    this->sendCmd_RUN(0, 305, file, BLOCK, {});
    this->dispatchAll();

    this->flushTelemetry();

    ASSERT_TLM_ControllerState(0, ControllerState::IDLE);
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(0));
    ASSERT_TLM_LastTrapReason(0, WasmSequencer_TrapReason::UNREACHABLE);
    this->removeFile("unreachable.wasm");
}

TEST_F(WasmSequencerTester, TelemetryCancelledCount) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 306, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);
    this->sendCmd_CANCEL(0, 307);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    this->flushTelemetry();

    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(0));
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(0));
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, TelemetryCommandsDispatchedAndFailed) {
    // cmd.wasm dispatches one command out cmdOut; feeding a non-OK response bumps
    // CommandsFailed while CommandsDispatched counts the dispatch.
    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 308, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::EXECUTION_ERROR);
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_CommandsDispatched(0, static_cast<U64>(1));
    ASSERT_TLM_CommandsFailed(0, static_cast<U64>(1));
    // The guest still ran to completion after the failed command response.
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(1));
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, TelemetryCommandOkDoesNotCountFailed) {
    // An OK command response dispatches but does not increment CommandsFailed.
    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 309, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_CommandsDispatched(0, static_cast<U64>(1));
    ASSERT_TLM_CommandsFailed(0, static_cast<U64>(0));
    this->removeFile("cmd.wasm");
}

// ----------------------------------------------------------------------
// Host functions: COMMAND (byte-fidelity of guest bytes -> cmdOut ComBuffer)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, CommandByteFidelityAndResume) {
    const Fw::String file = this->copyAsset("cmd.wasm");

    // RUN pauses when the guest calls cmd, dispatches the command out cmdOut,
    // and parks in AWAITING_RESPONSE until we feed a cmdResponseIn.
    this->sendCmd_RUN(0, 90, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
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
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, CommandTooBigTraps) {
    const Fw::String file = this->copyAsset("cmd_toobig.wasm");
    this->sendCmd_RUN(0, 91, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    // len 600 + packet descriptor > FW_COM_BUFFER_MAX_SIZE; the reported maxSize
    // is the payload budget after reserving the descriptor prefix.
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::COMMAND, 600,
                                 FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType));
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0, WasmSequencer_TrapReason::HOST,
                                 WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("cmd_toobig.wasm");
}

TEST_F(WasmSequencerTester, UnexpectedCmdResponseFromIdle) {
    // A cmdResponseIn while not awaiting a response is "unexpected". From IDLE
    // the state machine raises stmtUnexpected; it must not crash.
    this->invoke_to_cmdResponseIn(0, 0, 0, Fw::CmdResponse::OK);
    this->dispatchAll();
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// Host-function invalid-pointer error paths (guest passes an OOB pointer)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, CommandBadPointerFails) {
    // cmd() with an out-of-bounds buffer pointer: spacewasm_mem_read fails ->
    // HostFunctionInvalidPointer(COMMAND) -> stmtFailure -> IDLE.
    const Fw::String file = this->copyAsset("cmd_badptr.wasm");
    this->sendCmd_RUN(0, 92, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    // The OOB buffer read fails with ERR_MEM_OUT_OF_BOUNDS in the COMMAND path.
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::COMMAND,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 92, Fw::CmdResponse::EXECUTION_ERROR);
    // No command was actually dispatched.
    ASSERT_from_cmdOut_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("cmd_badptr.wasm");
}

TEST_F(WasmSequencerTester, EventBadPointerFails) {
    const Fw::String file = this->copyAsset("event_badptr.wasm");
    this->sendCmd_RUN(0, 93, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::EVENT,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("event_badptr.wasm");
}

TEST_F(WasmSequencerTester, ParameterBadPointerFails) {
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    const Fw::String file = this->copyAsset("prm_badptr.wasm");
    this->sendCmd_RUN(0, 94, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // getParam was queried, but writing the value into guest memory failed.
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::PARAMETER,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("prm_badptr.wasm");
}

TEST_F(WasmSequencerTester, TelemetryBadTimePointerFails) {
    // The time write (first mem_write) fails on an OOB time pointer.
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    const Fw::String file = this->copyAsset("tlm_badtimeptr.wasm");
    this->sendCmd_RUN(0, 95, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    // The time (first) mem_write fails on the OOB pointer in the TELEMETRY path.
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::TELEMETRY,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
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
    this->sendCmd_RUN(0, 96, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    // The value (second) mem_write fails on the OOB pointer in the TELEMETRY path.
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::TELEMETRY,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("tlm_badvalptr.wasm");
}

// ----------------------------------------------------------------------
// Sleep host functions + checkTimers wake path
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RelativeSleepWakes) {
    const Fw::String file = this->copyAsset("rsleep.wasm");

    // Start at t=0. rsleep asks for a 1s relative timer.
    this->setTestTime(Fw::Time(0, 0));
    this->sendCmd_RUN(0, 100, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_SLEEPING);

    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());

    // A checkTimers tick before the deadline does not wake it.
    this->setTestTime(Fw::Time(0, 500000));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_SLEEPING);

    // Past the deadline, the timer fires and the interpreter resumes/finishes.
    this->setTestTime(Fw::Time(2, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("rsleep.wasm");
}

TEST_F(WasmSequencerTester, AbsoluteSleepWakes) {
    const Fw::String file = this->copyAsset("asleep.wasm");

    this->setTestTime(Fw::Time(0, 0));
    this->sendCmd_RUN(0, 101, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());

    // asleep target is 10s from epoch.
    this->setTestTime(Fw::Time(20, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("asleep.wasm");
}

TEST_F(WasmSequencerTester, SleepTimeBaseMismatchFails) {
    const Fw::String file = this->copyAsset("rsleep.wasm");

    // Set the timer using a specific time base.
    this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 0, 0));
    this->sendCmd_RUN(0, 102, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());

    // Change the time base: comparison becomes INCOMPARABLE -> timeOpFailed.
    this->setTestTime(Fw::Time(TimeBase::TB_SC_TIME, 0, 5, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("rsleep.wasm");
}

TEST_F(WasmSequencerTester, SleepDurationOverflowFails) {
    // A guest sleep whose whole-seconds part (micros / 1e6) overflows the U32
    // seconds field of Fw::Time is rejected up front by wasmRsleep with
    // SleepDurationTooLarge -> TRAP (HOST trap reason), rather than silently
    // truncating the timer. The sequence fails to IDLE.
    const Fw::String file = this->copyAsset("sleep_overflow.wasm");
    this->sendCmd_RUN(0, 103, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SleepDurationTooLarge_SIZE(1);
    ASSERT_EVENTS_SleepDurationTooLarge(0, WasmSequencer_HostFunction::RSLEEP, static_cast<U64>(0x7FFFFFFFFFFFFFFFULL));
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::INTERPRETER_TRAP, 0, WasmSequencer_TrapReason::HOST,
                                 WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("sleep_overflow.wasm");
}

// ----------------------------------------------------------------------
// PAUSE / CONTINUE / CANCEL across the run state machine (loop.wasm)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, PauseThenContinueCompletes) {
    // Tiny fuel makes the busy-loop span many OUT_OF_FUEL cycles so PAUSE can
    // land between statements deterministically.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 110, file, NO_BLOCK, {});
    // Advance into the running loop.
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    // Request a pause; the machine pauses before the next spin.
    this->sendCmd_PAUSE(0, 111);
    this->dispatchUntilEngineState(EngineState::RUNNING_PAUSED);
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_PAUSED);
    ASSERT_EVENTS_SequencePaused_SIZE(1);

    // Continue: it resumes spinning and eventually finishes.
    this->sendCmd_CONTINUE(0, 112);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, CancelWhileSpinning) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 120, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    // CANCEL is synchronous (responds OK immediately) and returns to IDLE.
    this->sendCmd_CANCEL(0, 121);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, CancelWhilePaused) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 122, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    this->sendCmd_PAUSE(0, 123);
    this->dispatchUntilEngineState(EngineState::RUNNING_PAUSED);

    this->sendCmd_CANCEL(0, 124);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, CancelWhileAwaitingResponse) {
    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 125, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // CANCEL from AWAITING_RESPONSE clears the pending host function and returns
    // to IDLE.
    this->sendCmd_CANCEL(0, 126);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, CancelFromReadyReturnsIdle) {
    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_LOAD(0, 130, file);
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_CANCEL(0, 131);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, CancelFromIdleStaysIdle) {
    // CANCEL from IDLE is a self-transition back to IDLE (cmd_CANCEL IDLE branch): it
    // re-enters IDLE (resetStore) without emitting a cancellation and does not crash.
    this->sendCmd_CANCEL(0, 132);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(0);
    // Re-entering IDLE reallocated the store; the diagnostic event reports the
    // configured module capacity. (Asserted here rather than at construction
    // because the store is first created before the test harness connects ports.)
    ASSERT_EVENTS_StoreAllocationSucceeded_SIZE(1);
    ASSERT_EVENTS_StoreAllocationSucceeded(0, WasmSequencerConfig::MAX_GUEST_MODULES);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, UnexpectedCmdResponseWhilePausedFails) {
    // A cmdResponseIn while RUNNING_PAUSED (not awaiting a host command) is unexpected
    // and fails the sequence (stmtUnexpected from RUNNING_PAUSED -> IDLE).
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 133, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    this->sendCmd_PAUSE(0, 134);
    this->dispatchUntilEngineState(EngineState::RUNNING_PAUSED);

    // Tag the response with the current sequence's cmdUid so it isn't dismissed as
    // a late reply from an old sequence; it is unexpected here and fails.
    this->invoke_to_cmdResponseIn(0, 0, this->currentCmdUid(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, PauseAtHostFunctionThenContinueResumes) {
    // cmd.wasm reaches the cmd host function on its very first spin. dispatchUntilState
    // stops at RUNNING_SPINNING with the entry `entered` signal still queued (before that
    // first spin runs), so a PAUSE queued now is processed right after the spin that hits
    // the host function: RUNNING_SPINNING_HOST_FUNCTION sees pendingPause and enters
    // RUNNING_PAUSED with the host function still pending, rather than dispatching it.
    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 135, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    this->sendCmd_PAUSE(0, 136);
    this->dispatchUntilEngineState(EngineState::RUNNING_PAUSED);
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_PAUSED);
    ASSERT_EVENTS_SequencePaused_SIZE(1);
    // The command has not been dispatched: the pause pre-empted the host-function
    // dispatch, and the pending host function is retained across the pause.
    ASSERT_from_cmdOut_SIZE(0);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::COMMAND);

    // CONTINUE resumes into the pending host function (PAUSED_RESUME host-function
    // branch): the command is dispatched and the machine awaits its response.
    this->sendCmd_CONTINUE(0, 137);
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_from_cmdOut_SIZE(1);

    // Feed the response so the sequence finishes cleanly.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_from_cmdOut_SIZE(1);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, CheckTimersWhileAwaitingWithoutTimer) {
    // A COMMAND host function parks in AWAITING_RESPONSE with NO pending timer (unlike
    // sleep). A checkTimers tick there takes the CHECK_TIMERS else branch
    // (guard_pendingTimer false -> checkStatementTimeout only) and stays awaiting.
    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 138, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_FALSE(this->hasPendingTimer());

    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    // No timer to wake it: it remains awaiting the command response.
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Response still resumes it to completion.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("cmd.wasm");
}

// ----------------------------------------------------------------------
// Statement timeout for blocking async host functions
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, StatementTimeoutFailsAwaitingCommand) {
    // With STATEMENT_TIMEOUT_SECS set, a command that never gets a response times
    // out on a checkTimers tick past the deadline and fails the sequence.
    this->paramSet_STATEMENT_TIMEOUT_SECS(5.0f, Fw::ParamValid::VALID);
    this->component.loadParameters();  // refresh the component's cached parameter value
    this->setTestTime(Fw::Time(0, 0));

    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 400, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // A tick before the deadline keeps it awaiting.
    this->setTestTime(Fw::Time(3, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // A tick past the deadline times the statement out.
    this->setTestTime(Fw::Time(10, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 400, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, StatementTimeoutTimeIncomparableFails) {
    // While AWAITING_RESPONSE for a COMMAND (WAITING, not sleeping), a checkTimers
    // tick runs checkTimeout. If the current time is incomparable to the statement
    // start (the time base/context changed), the deadline cannot be evaluated:
    // checkTimeout raises hostResponseTimeIncomparable -> TIMER_INCOMPARABLE exit
    // and the sequence fails to IDLE. This is the statement-timeout analogue of
    // SleepTimeBaseMismatchFails (which exercises the SLEEPING/checkSleepTimers path).
    this->paramSet_STATEMENT_TIMEOUT_SECS(5.0f, Fw::ParamValid::VALID);
    this->component.loadParameters();
    this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 0, 0));

    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 401, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Change the time base: comparison against the statement-start deadline becomes
    // INCOMPARABLE.
    this->setTestTime(Fw::Time(TimeBase::TB_SC_TIME, 0, 10, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed(0, 0, WasmSequencer_ExitReason::TIMER_INCOMPARABLE, 0, WasmSequencer_TrapReason::NONE,
                                 WasmSequencer_HostFunction::COMMAND);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 401, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, StatementTimeoutDisabledByDefault) {
    // With STATEMENT_TIMEOUT_SECS left at its 0 default, no amount of elapsed time
    // times out an awaiting command.
    this->setTestTime(Fw::Time(0, 0));

    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 401, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Advance well past any plausible timeout and tick: still awaiting.
    this->setTestTime(Fw::Time(100000, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // The response still resumes it cleanly.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, StatementTimeoutFailsAwaitingSerialReply) {
    // The timeout applies equally to a blocking async serial port awaiting a reply.
    this->paramSet_STATEMENT_TIMEOUT_SECS(2.0f, Fw::ParamValid::VALID);
    this->component.loadParameters();  // refresh the component's cached parameter value
    this->setTestTime(Fw::Time(0, 0));

    const Fw::String file = this->copyAsset("serial_async.wasm");
    this->sendCmd_RUN(0, 402, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::ASYNC_PORT);

    this->setTestTime(Fw::Time(30, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 402, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("serial_async.wasm");
}

// ----------------------------------------------------------------------
// Command context (cmdUid): late-reply and wrong-instance detection
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, LateCmdResponseFromOldSequenceIgnored) {
    // A response tagged with a previous sequence's cmdUid is a nominal late reply
    // (e.g. after a CANCEL). It is reported, not failed, and does not disturb the
    // sequence currently awaiting its own response.
    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 410, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Craft a cmdUid from an older sequence index (current - 1).
    const U32 currentUid = this->currentCmdUid();
    const U16 oldSeq = static_cast<U16>(((currentUid >> 16) & 0xFFFF) - 1);
    const U32 staleUid = static_cast<U32>((static_cast<U32>(oldSeq) << 16) | (currentUid & 0xFFFF));

    this->invoke_to_cmdResponseIn(0, 0, staleUid, Fw::CmdResponse::OK);
    this->dispatchAll();

    // Still awaiting; the stale reply was ignored with a warning.
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EVENTS_CmdResponseFromOldSequence_SIZE(1);
    // The event must carry the perturbed opcode/response and the old-vs-current
    // sequence indices, not just fire.
    ASSERT_EVENTS_CmdResponseFromOldSequence(0, 0, Fw::CmdResponse::OK, oldSeq,
                                             static_cast<U16>((currentUid >> 16) & 0xFFFF));

    // The correct reply still completes the sequence.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, WrongCmdResponseIndexFails) {
    // A response from the current sequence but a different command instance (wrong
    // low-half index) is an integrity error and fails the sequence.
    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 411, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Keep the current sequence index, but perturb the command index.
    const U32 currentUid = this->currentCmdUid();
    const U32 wrongUid = static_cast<U32>((currentUid & 0xFFFF0000) | ((currentUid + 1) & 0xFFFF));

    this->invoke_to_cmdResponseIn(0, 0, wrongUid, Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_WrongCmdResponseIndex_SIZE(1);
    // The event must carry the opcode/response and the actual-vs-expected command
    // indices, not just fire.
    ASSERT_EVENTS_WrongCmdResponseIndex(0, 0, Fw::CmdResponse::OK, static_cast<U16>((wrongUid) & 0xFFFF),
                                        static_cast<U16>(currentUid & 0xFFFF));
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 411, Fw::CmdResponse::EXECUTION_ERROR);
    this->removeFile("cmd.wasm");
}

// ----------------------------------------------------------------------
// Commands rejected / queued while a sequence is running
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RunWhileRunningRejected) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 150, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    // RUN is only valid from IDLE/READY; from RUNNING it is rejected
    // (RUN_cmdHandler invalid-state guard) without disturbing the running loop.
    // The original NO_BLOCK RUN already responded OK at load (index 0); the
    // rejected RUN lands as an EXECUTION_ERROR (index 1). The loop still finishes.
    this->sendCmd_RUN(0, 151, file, NO_BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_InvalidCommand_SIZE(1);
    ASSERT_EVENTS_InvalidCommand(0, WasmSequencer_ControllerStateMachine_State::RUNNING_MAIN);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 150, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_RUN, 151, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, LoadWhileRunningRejected) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 152, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    // LOAD_NAME invalid-state guard (only IDLE/READY accepted). The original
    // NO_BLOCK RUN responded OK at load (index 0); the rejected LOAD is
    // EXECUTION_ERROR (index 1). The loop still finishes.
    this->sendCmd_LOAD(0, 153, file);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_InvalidCommand_SIZE(1);
    ASSERT_EVENTS_InvalidCommand(0, WasmSequencer_ControllerStateMachine_State::RUNNING_MAIN);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 152, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_LOAD, 153, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, WaitWhileRunningQueuesUntilFinish) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 160, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    // WAIT while running enqueues on the finish queue (default branch of
    // WAIT_cmdHandler) and only responds once the loop completes.
    this->sendCmd_WAIT(0, 161);
    this->dispatchUntilControllerState(ControllerState::READY);

    // The RUN already responded OK at load (NO_BLOCK); the WAIT responds OK on
    // finish. Both land in the cmd-response history.
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 160, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_WAIT, 161, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, UnexpectedCmdResponseWhileSpinningFails) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 170, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    // A cmdResponseIn while spinning (not awaiting a host command) is "unexpected"
    // and fails the running sequence (stmtUnexpected -> report_seqFailed). Tag it
    // with the current sequence's cmdUid so it is not dismissed as a late reply.
    this->invoke_to_cmdResponseIn(0, 0, this->currentCmdUid(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, UnexpectedCmdResponseWhileAwaitingSleepFails) {
    // While AWAITING_RESPONSE for a NON-command host function (a sleep), an incoming
    // cmdResponseIn is unexpected: cmdResponseIn_handler sees the pending kind is not
    // COMMAND and raises stmtUnexpected from RUNNING_AWAITING_RESPONSE -> IDLE.
    const Fw::String file = this->copyAsset("rsleep.wasm");
    this->setTestTime(Fw::Time(0, 0));
    this->sendCmd_RUN(0, 172, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());

    // Tag with the current sequence's cmdUid so it reaches the "unexpected" path
    // (the pending host function is a sleep, not a COMMAND) rather than being
    // dismissed as a late reply.
    this->invoke_to_cmdResponseIn(0, 0, this->currentCmdUid(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("rsleep.wasm");
}

TEST_F(WasmSequencerTester, LoadWhileReadyReloads) {
    // A LOAD from READY is accepted (cmd_LOAD READY branch: pendLoad + enter LOADING)
    // and reloads the module, ending back in READY.
    const Fw::String file = this->copyAsset("empty.wasm");
    this->sendCmd_LOAD(0, 173, file);
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_LOAD(0, 174, file);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 173, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_LOAD, 174, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("empty.wasm");
}

TEST_F(WasmSequencerTester, PauseWhileAwaitingResponseIsPending) {
    // PAUSE from RUNNING_AWAITING_RESPONSE records a pending pause (cmd_PAUSE
    // AWAITING_RESPONSE branch: action_pendPause) without leaving the state. When the
    // command response arrives, the pending pause takes effect at PAUSE_CHECK.
    const Fw::String file = this->copyAsset("cmd.wasm");
    this->sendCmd_RUN(0, 175, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    this->sendCmd_PAUSE(0, 176);
    this->dispatchAll();
    // Still awaiting the response; the pause is only pending.
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Feeding the response now resolves the pending pause -> RUNNING_PAUSED.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilEngineState(EngineState::RUNNING_PAUSED);
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_PAUSED);
    ASSERT_EVENTS_SequencePaused_SIZE(1);

    // Continue to completion.
    this->sendCmd_CONTINUE(0, 177);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("cmd.wasm");
}

TEST_F(WasmSequencerTester, PauseWhilePausedIsIdempotent) {
    // PAUSE from RUNNING_PAUSED is a self-transition that just re-records the pending
    // pause (cmd_PAUSE RUNNING_PAUSED branch: action_pendPause); it stays paused.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 178, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    this->sendCmd_PAUSE(0, 179);
    this->dispatchUntilEngineState(EngineState::RUNNING_PAUSED);

    this->sendCmd_PAUSE(0, 180);
    this->dispatchAll();
    // The duplicate PAUSE is a self-transition: still paused, no extra broken event.
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_PAUSED);
    ASSERT_EVENTS_SequencePaused_SIZE(1);

    // CANCEL cleanly tears down the paused sequence.
    this->sendCmd_CANCEL(0, 181);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    this->removeFile("loop.wasm");
}

TEST_F(WasmSequencerTester, WaitFinishQueueOverflow) {
    // The blocking-finish queue (m_pendingFinishCmds) is 8 deep. Fill it with
    // WAITs while a sequence is running; the 9th overflows and is rejected with
    // TooManyBlockingCommands + EXECUTION_ERROR (WAIT_cmdHandler enqueue-failure).
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 180, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    // Queue nine WAITs up front (test queue depth is 20) so they are all handled
    // while the loop is still spinning: eight enqueue, the ninth overflows.
    for (U32 i = 0; i < 9; i++) {
        this->sendCmd_WAIT(0, 181 + i);
    }
    this->dispatchUntilControllerState(ControllerState::READY);

    // All queued WAITs are drained on finish and the sequence returns to READY.
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_TooManyBlockingCommands_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

// ----------------------------------------------------------------------
// Continue is a no-op while already running
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ContinueWhileSpinningIsOk) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String file = this->copyAsset("loop.wasm");
    this->sendCmd_RUN(0, 140, file, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);

    this->sendCmd_CONTINUE(0, 141);
    this->dispatchUntilControllerState(ControllerState::READY);
    // CONTINUE while running just responds OK; the loop still finishes.
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("loop.wasm");
}

// ----------------------------------------------------------------------
// Host functions: SYNC_PORT (byte-fidelity of guest bytes -> serialOut buffer)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, SerialSyncByteFidelityAndResume) {
    const Fw::String file = this->copyAsset("serial_sync.wasm");

    // The guest calls serial_sync on port index 1 with an 8-byte pattern. The host copies
    // the payload out, emits it on serialOut[1], and resumes immediately (no reply awaited),
    // so the sequence runs straight to completion.
    this->sendCmd_RUN(0, 200, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);

    // The payload round-trips verbatim on the requested port.
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(1));
    ASSERT_EQ(this->lastSerialOutPort, static_cast<FwIndexType>(1));
    const U8 expectedPattern[8] = {0xde, 0xad, 0xbe, 0xef, 0x01, 0x02, 0x03, 0x04};
    ASSERT_EQ(this->lastSerialOutSize, static_cast<FwSizeType>(sizeof expectedPattern));
    for (FwSizeType i = 0; i < sizeof expectedPattern; i++) {
        ASSERT_EQ(this->lastSerialOutData[i], expectedPattern[i]) << "payload byte " << i;
    }
    this->removeFile("serial_sync.wasm");
}

TEST_F(WasmSequencerTester, SerialSyncInvalidPortTraps) {
    // serial_sync with an out-of-range port index is rejected in the host function with a
    // trap before any port invocation.
    const Fw::String file = this->copyAsset("serial_sync_badport.wasm");
    this->sendCmd_RUN(0, 201, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPort_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPort(0, WasmSequencer_HostFunction::SYNC_PORT, 5, 5);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
    this->removeFile("serial_sync_badport.wasm");
}

TEST_F(WasmSequencerTester, SerialSyncPayloadTooLargeTraps) {
    // serial_sync with a payload larger than MAX_SERIAL_PORT_SIZE is rejected in the host
    // function with a trap (BufferTooLarge) before any port invocation.
    const Fw::String file = this->copyAsset("serial_sync_toobig.wasm");
    this->sendCmd_RUN(0, 202, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::SYNC_PORT, 300,
                                 Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
    this->removeFile("serial_sync_toobig.wasm");
}

TEST_F(WasmSequencerTester, SerialSyncBadPointerFails) {
    // serial_sync with an out-of-bounds data pointer: spacewasm_mem_read fails ->
    // HostFunctionInvalidPointer(SYNC_PORT) -> stmtFailure -> IDLE. The port is
    // never invoked.
    const Fw::String file = this->copyAsset("serial_sync_badptr.wasm");
    this->sendCmd_RUN(0, 206, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::SYNC_PORT,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
    this->removeFile("serial_sync_badptr.wasm");
}

// ----------------------------------------------------------------------
// Host functions: ASYNC_PORT (emit on serialOut, block for serialReply)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, SerialAsyncEmitsAndBlocksForReply) {
    const Fw::String file = this->copyAsset("serial_async.wasm");

    // The guest calls serial_async on port index 2. The host emits the payload on
    // serialOut[2] and parks in AWAITING_RESPONSE until a reply arrives on serialReply[2].
    this->sendCmd_RUN(0, 203, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->engineState(), EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::ASYNC_PORT);

    // The request payload round-tripped verbatim on the requested port.
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(1));
    ASSERT_EQ(this->lastSerialOutPort, static_cast<FwIndexType>(2));
    const U8 expectedRequest[4] = {0x11, 0x22, 0x33, 0x44};
    ASSERT_EQ(this->lastSerialOutSize, static_cast<FwSizeType>(sizeof expectedRequest));
    for (FwSizeType i = 0; i < sizeof expectedRequest; i++) {
        ASSERT_EQ(this->lastSerialOutData[i], expectedRequest[i]) << "request byte " << i;
    }

    // Deliver a 4-byte reply on serialReply[2]. The guest asserts (via unreachable) that
    // these exact bytes land in its return buffer, so a clean finish proves the round trip.
    const U8 replyBytes[4] = {0x11, 0x22, 0x33, 0x44};
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE> reply(replyBytes, sizeof replyBytes);
    this->invoke_to_serialReply(2, reply);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    this->removeFile("serial_async.wasm");
}

TEST_F(WasmSequencerTester, SerialAsyncReplyTooLargeFails) {
    // A reply larger than the guest's return buffer (return_size == 4) cannot be delivered:
    // serialReply_handler fails the statement (BufferTooSmall) and the sequence fails.
    const Fw::String file = this->copyAsset("serial_async.wasm");
    this->sendCmd_RUN(0, 204, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    const U8 replyBytes[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE> reply(replyBytes, sizeof replyBytes);
    this->invoke_to_serialReply(2, reply);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    this->removeFile("serial_async.wasm");
}

TEST_F(WasmSequencerTester, SerialAsyncBadReturnPointerFails) {
    // serial_async emits the request fine, but the guest's return pointer is out of bounds.
    // When the reply arrives, serialReply_handler's spacewasm_mem_write fails ->
    // HostFunctionInvalidPointer(ASYNC_PORT) -> stmtFailure -> IDLE.
    const Fw::String file = this->copyAsset("serial_async_badret.wasm");
    this->sendCmd_RUN(0, 209, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(1));

    const U8 replyBytes[4] = {0x55, 0x66, 0x77, 0x88};
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE> reply(replyBytes, sizeof replyBytes);
    this->invoke_to_serialReply(2, reply);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::ASYNC_PORT,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    this->removeFile("serial_async_badret.wasm");
}

TEST_F(WasmSequencerTester, SerialReplyWhileNotAwaitingIsUnexpected) {
    // A serialReply that arrives while the sequencer is not awaiting a response (here from
    // IDLE) is unexpected: serialReply_handler raises stmtUnexpected. It must not
    // crash and must stay in IDLE.
    const U8 replyBytes[4] = {0x11, 0x22, 0x33, 0x44};
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE> reply(replyBytes, sizeof replyBytes);
    this->invoke_to_serialReply(2, reply);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, SerialReplyOnWrongPortIsUnexpected) {
    // A reply on a port that does not match the pending async invocation is unexpected and
    // fails the sequence (stmtUnexpected).
    const Fw::String file = this->copyAsset("serial_async.wasm");
    this->sendCmd_RUN(0, 205, file, BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_AWAITING_RESPONSE_WAITING);

    const U8 replyBytes[4] = {0x11, 0x22, 0x33, 0x44};
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE> reply(replyBytes, sizeof replyBytes);
    // Pending invocation is on port 2; reply on port 0.
    this->invoke_to_serialReply(0, reply);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    this->removeFile("serial_async.wasm");
}

TEST_F(WasmSequencerTester, SerialAsyncInvalidPortTraps) {
    // serial_async with an out-of-range port index is rejected in the host function with a
    // trap before any port invocation.
    const Fw::String file = this->copyAsset("serial_async_badport.wasm");
    this->sendCmd_RUN(0, 206, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPort_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPort(0, WasmSequencer_HostFunction::ASYNC_PORT, 5, 5);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
    this->removeFile("serial_async_badport.wasm");
}

TEST_F(WasmSequencerTester, SerialAsyncPayloadTooLargeTraps) {
    // serial_async with a payload larger than MAX_SERIAL_PORT_SIZE is rejected in the host
    // function with a trap (BufferTooLarge) before any port invocation.
    const Fw::String file = this->copyAsset("serial_async_toobig.wasm");
    this->sendCmd_RUN(0, 208, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::ASYNC_PORT, 300,
                                 Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
    this->removeFile("serial_async_toobig.wasm");
}

TEST_F(WasmSequencerTester, SerialAsyncBadPointerFails) {
    // serial_async with an out-of-bounds data pointer: spacewasm_mem_read fails ->
    // HostFunctionInvalidPointer(ASYNC_PORT) -> stmtFailure -> IDLE, without
    // emitting on serialOut or awaiting a reply.
    const Fw::String file = this->copyAsset("serial_async_badptr.wasm");
    this->sendCmd_RUN(0, 207, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::ASYNC_PORT,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    ASSERT_EVENTS_SequenceFailed_SIZE(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
    this->removeFile("serial_async_badptr.wasm");
}

// ----------------------------------------------------------------------
// End-to-end lifecycle: many loads/runs with failures interspersed, driving
// the controller through IDLE <-> READY <-> RUNNING repeatedly and checking
// that the store recovers from every failure mode and the cumulative counters
// track correctly across the whole session.
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, LifecycleMultipleLoadsWithFailures) {
    // Tiny fuel so loop.wasm spins across many cycles and CANCEL lands mid-run.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    const Fw::String empty = this->copyAsset("empty.wasm");
    const Fw::String unreachable = this->copyAsset("unreachable.wasm");
    const Fw::String malformed = this->copyAsset("malformed.wasm");
    const Fw::String loop = this->copyAsset("loop.wasm");
    const Fw::String exit0 = this->copyAsset("exit0.wasm");
    const Fw::String panic = this->copyAsset("panic.wasm");

    // Sequence-success/cancel events map 1:1 with those failure modes, but the
    // "failed" telemetry counter aggregates two distinct events: SequenceFailed
    // (an execution failure) and ModuleLoadFailed (a load failure). Track the
    // execution-failure event count separately from the cumulative counter.
    U32 succeeded = 0;
    U32 cancelled = 0;
    U32 seqFailedEvents = 0;
    U32 failedCount = 0;  // telemetry SequencesFailed (execution + load failures)

    // 1. RUN empty -> success, land READY.
    this->sendCmd_RUN(0, 1, empty, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);
    succeeded++;
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(succeeded);

    // 2. RUN unreachable from READY -> traps, store is torn down to IDLE. Proves a
    //    failure after a prior success returns to a clean IDLE (not a wedged store).
    this->sendCmd_RUN(0, 2, unreachable, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::IDLE);
    seqFailedEvents++;
    failedCount++;
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(seqFailedEvents);

    // 3. LOAD empty from IDLE -> READY, but no run: counters unchanged.
    this->sendCmd_LOAD(0, 3, empty);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);

    // 4. RUN malformed from READY -> load fails (resetStore then load). A failed
    //    load invalidates the store and returns to IDLE. It reports ModuleLoadFailed
    //    (not SequenceFailed) but still counts toward SequencesFailed.
    this->sendCmd_RUN(0, 4, malformed, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::IDLE);
    failedCount++;
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    ASSERT_EVENTS_SequenceFailed_SIZE(seqFailedEvents);  // unchanged: load fail is not a SequenceFailed

    // 5. RUN loop NO_BLOCK, then CANCEL mid-spin -> cancelled, back to IDLE.
    this->sendCmd_RUN(0, 5, loop, NO_BLOCK, {});
    this->dispatchUntilEngineState(EngineState::RUNNING_SPINNING);
    this->sendCmd_CANCEL(0, 6);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    cancelled++;
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(cancelled);

    // 6. RUN exit0 -> clean exit(0) counts as success, land READY. Proves the
    //    store recovered from the cancel.
    this->sendCmd_RUN(0, 7, exit0, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);
    succeeded++;
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(succeeded);

    // 7. RUN panic from READY -> host panic, back to IDLE.
    this->sendCmd_RUN(0, 8, panic, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::IDLE);
    seqFailedEvents++;
    failedCount++;
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFailed_SIZE(seqFailedEvents);

    // Cumulative counters across the whole session line up with the per-phase
    // tallies, and every command got exactly one response.
    this->flushTelemetry();
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(succeeded));
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(failedCount));
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(cancelled));

    // Every RUN/LOAD command issued got an OK or EXECUTION_ERROR response.
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 1, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_RUN, 2, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_CMD_RESPONSE(2, OPCODE_LOAD, 3, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(3, OPCODE_RUN, 4, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_CMD_RESPONSE(4, OPCODE_RUN, 5, Fw::CmdResponse::OK);  // NO_BLOCK: OK at load
    ASSERT_CMD_RESPONSE(5, OPCODE_CANCEL, 6, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(6, OPCODE_RUN, 7, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(7, OPCODE_RUN, 8, Fw::CmdResponse::EXECUTION_ERROR);

    this->removeFile("empty.wasm");
    this->removeFile("unreachable.wasm");
    this->removeFile("malformed.wasm");
    this->removeFile("loop.wasm");
    this->removeFile("exit0.wasm");
    this->removeFile("panic.wasm");
}

}  // namespace Svc

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
