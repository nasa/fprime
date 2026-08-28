// ======================================================================
// \title  WasmSequencerTestMain.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component test main function
// ======================================================================

#include "WasmSequencerGTestBase.hpp"
#include "WasmSequencerTester.hpp"

#include <cstring>

#include "Fw/Port/InputPortBase.hpp"
#include "Fw/Test/UnitTestAssert.hpp"
#include "WasmSequencer_ControllerStateMachine_StateEnumAc.hpp"
#include "config/FwPacketDescriptorTypeAliasAc.h"
#include "spacewasm.h"

namespace Svc {

using ControllerState = WasmSequencerTester::ControllerState;
using InterpreterState = WasmSequencerTester::InterpreterState;

//! A serial input port stub whose invokeSerial always reports a serialization failure.
//! Connected to a serialOut[] index to drive the component's "serial port send failed"
//! error path (serialOut_out returning non-OK), which the normal tester InputSerializePort
//! -- always returning FW_SERIALIZE_OK -- cannot exercise.
class FailingSerialInputPort final : public Fw::InputPortBase {
  public:
    FailingSerialInputPort() : Fw::InputPortBase() { this->init(); }
    Fw::SerializeStatus invokeSerial(Fw::LinearBufferBase&) override { return Fw::FW_SERIALIZE_NO_ROOM_LEFT; }
};

// Convenience: the two block modes.
static const Svc::BlockState BLOCK(Svc::BlockState::BLOCK);
static const Svc::BlockState NO_BLOCK(Svc::BlockState::NO_BLOCK);

// ----------------------------------------------------------------------
// Construction / initial state
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, InitialStateIsIdle) {
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // No pending work on a fresh component.
    ASSERT_FALSE(this->hasPendingTimer());
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// LOAD (nominal + failures)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, LoadEmptyModuleReady) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 10, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 10, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadResolvesAgainstSeqBaseDir) {
    // With SEQ_BASE_DIR set, the base dir is prepended to the requested (bare) file
    // name. A base dir that already ends in '/' has no extra separator inserted, so a
    // base dir of "./" resolves "empty.wasm" -> "./empty.wasm" (copyAsset stages the
    // module in the CWD), which opens successfully.
    this->paramSet_SEQ_BASE_DIR(Fw::ParamString("./"), Fw::ParamValid::VALID);
    this->component.loadParameters();

    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 11, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 11, Fw::CmdResponse::OK);
    ASSERT_EVENTS_FileOpenError_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadAgainstMissingBaseDirFailsToOpen) {
    // A non-empty base dir that does not contain the file resolves to a path that
    // cannot be opened; the load fails with FileOpenError and returns to IDLE.
    this->paramSet_SEQ_BASE_DIR(Fw::ParamString("no_such_dir/"), Fw::ParamValid::VALID);
    this->component.loadParameters();

    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 12, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_FileOpenError_SIZE(1);
    // The path that failed to open is the base dir joined with the file name.
    ASSERT_STREQ(this->eventHistory_FileOpenError->at(0).fileName.toChar(), "no_such_dir/empty.wasm");
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 12, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(WasmSequencerTester, LoadInsertsSeparatorForBaseDirWithoutTrailingSlash) {
    // A base dir without a trailing '/' must still resolve to "<base>/<file>" rather
    // than concatenating verbatim ("<base><file>"). The verbatim join both mis-resolved
    // ordinary names and could escape the containment boundary ("seqs" + "_priv/x.wasm"
    // -> "seqs_priv/x.wasm", a sibling directory). Here the same missing directory as
    // the previous test is configured without the trailing slash and must resolve to
    // the identical path.
    this->paramSet_SEQ_BASE_DIR(Fw::ParamString("no_such_dir"), Fw::ParamValid::VALID);
    this->component.loadParameters();

    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 13, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_FileOpenError_SIZE(1);
    // Exactly one '/' is inserted between the base dir and the file name.
    ASSERT_STREQ(this->eventHistory_FileOpenError->at(0).fileName.toChar(), "no_such_dir/empty.wasm");
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 13, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(WasmSequencerTester, LoadRejectsPathTraversalOutsideBaseDir) {
    // With SEQ_BASE_DIR configured it is a containment boundary: a ground-supplied
    // file name containing a ".." component could escape the base dir. The load is
    // rejected up front with SequenceFilePathNotContained (no open is attempted)
    // instead of resolving a path outside the configured base directory.
    this->paramSet_SEQ_BASE_DIR(Fw::ParamString("seqs"), Fw::ParamValid::VALID);
    this->component.loadParameters();

    this->sendCmd_LOAD(0, 20, Fw::CmdStringArg("../../etc/evil.wasm"), Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceFilePathNotContained_SIZE(1);
    ASSERT_STREQ(this->eventHistory_SequenceFilePathNotContained->at(0).baseDir.toChar(), "seqs");
    ASSERT_STREQ(this->eventHistory_SequenceFilePathNotContained->at(0).fileName.toChar(), "../../etc/evil.wasm");
    // Rejected before any open is attempted.
    ASSERT_EVENTS_FileOpenError_SIZE(0);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 20, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(WasmSequencerTester, SeqRunPathTooLongEmitsEvent) {
    // Joining SEQ_BASE_DIR with a long file name overflows the internal file-path
    // buffer (Fw::String, 256 bytes). Command string args are too short to reach
    // that, so the RUN arrives over seqRunIn, which carries a longer file name. The
    // overflow is reported via SequenceFilePathTooLong and the load is failed back to
    // IDLE without attempting to open the (truncated) path.
    char baseChars[36];
    for (FwSizeType i = 0; i < 35; i++) {
        baseChars[i] = 'b';
    }
    baseChars[35] = '\0';
    this->paramSet_SEQ_BASE_DIR(Fw::ParamString(baseChars), Fw::ParamValid::VALID);
    this->component.loadParameters();

    char nameChars[231];
    for (FwSizeType i = 0; i < 230; i++) {
        nameChars[i] = 'a';
    }
    nameChars[230] = '\0';
    const Fw::String longName(nameChars);

    this->invoke_to_seqRunIn(0, longName, Svc::SeqArgs());
    this->dispatchAll();

    ASSERT_EVENTS_SequenceFilePathTooLong_SIZE(1);
    ASSERT_STREQ(this->eventHistory_SequenceFilePathTooLong->at(0).baseDir.toChar(), baseChars);
    // The event carries (a prefix of) the requested file name -- the event's string
    // arg may be shorter than the full requested name, so check it is a non-empty run
    // of the requested characters rather than pinning the exact truncation boundary.
    const Fw::StringBase& evtName = this->eventHistory_SequenceFilePathTooLong->at(0).fileName;
    ASSERT_GT(evtName.length(), static_cast<FwSizeType>(0));
    for (FwSizeType i = 0; i < evtName.length(); i++) {
        ASSERT_EQ(evtName.toChar()[i], 'a');
    }
    // The interpreter never starts; the controller settles back in IDLE.
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
}

TEST_F(WasmSequencerTester, LoadNamedModuleReady) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 11, file, Fw::CmdStringArg("mod"));
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 11, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadStartModuleRespondsOk) {
    // A LOAD whose module carries a (running) Wasm start function drives
    // STARTING -> startInvoked -> RUNNING and spins the start to completion. The
    // load command must be answered when the start finishes and we settle in
    // READY -- not left dangling (which previously also wedged the single load-cmd
    // slot, tripping an assert on the next load).
    StagedAsset file_asset(*this, "start.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 40, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 40, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadStartModuleTrapRespondsError) {
    // The module's start function begins running then traps (unreachable). The
    // pending load command must receive EXECUTION_ERROR as we fall back to IDLE.
    StagedAsset file_asset(*this, "start_trap.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 41, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // A LOAD-with-start whose start function traps fails during the start phase; a
    // trap is reported via SequenceTrapped regardless of the phase it occurred in.
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::START, WasmSequencer_TrapReason::UNREACHABLE);
    ASSERT_CMD_RESPONSE_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 41, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);

    // Regression for the SequencesFailed double-count: a start-phase runtime failure
    // once ran both reportModuleStartFailed and respond_ERROR (each bumping the
    // counter). It must now count exactly once, and never as a cancel.
    this->flushTelemetry();
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(0));
}

TEST_F(WasmSequencerTester, LoadStartModuleTwiceDoesNotWedge) {
    // Regression: a LOAD-with-start must settle back in READY on completion so a
    // second LOAD-with-start is accepted rather than rejected as BUSY (the SM only
    // rejects load/run signals while a prior request is still in flight).
    StagedAsset file_asset(*this, "start.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 42, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 42, Fw::CmdResponse::OK);

    // Second load from READY reuses the store and runs the start again. The
    // component is already in READY, so drain the queue rather than waiting for a
    // state change that has effectively already happened.
    this->sendCmd_LOAD(0, 43, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE_SIZE(2);
    ASSERT_CMD_RESPONSE(1, OPCODE_LOAD, 43, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadFileNotFound) {
    // No copyAsset: the file simply does not exist in the CWD.
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
    this->removeFile("does_not_exist.wasm");
    this->sendCmd_LOAD(0, 12, Fw::CmdStringArg("does_not_exist.wasm"), Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // Open failure surfaces as FileOpenError (naming the missing file) then a
    // failed load. The status arg is the Os::File open status; assert only the
    // file name here since the numeric status is platform-dependent.
    ASSERT_EVENTS_FileOpenError_SIZE(1);
    ASSERT_STREQ(this->eventHistory_FileOpenError->at(0).fileName.toChar(), "does_not_exist.wasm");
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(0);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 12, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(WasmSequencerTester, LoadMalformedMagic) {
    StagedAsset file_asset(*this, "malformed.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 13, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // A bad wasm magic number decodes to ERR_MALFORMED_MAGIC.
    ASSERT_EVENTS_ModuleLoadFailed(0, WasmSequencer_Status::ERR_MALFORMED_MAGIC);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 13, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadTruncated) {
    StagedAsset file_asset(*this, "truncated.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 14, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // A module that ends mid-stream fails to decode with ERR_EOF.
    ASSERT_EVENTS_ModuleLoadFailed(0, WasmSequencer_Status::ERR_EOF);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadBigMemFailsAllocator) {
    // Requests more guest pages than the 2048-byte guest pool can back.
    StagedAsset file_asset(*this, "bigmem.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 15, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // The oversized guest memory request fails the allocator with
    // ERR_GUEST_MEMORY_ALLOC_FAILED.
    ASSERT_EVENTS_ModuleLoadFailed(0, WasmSequencer_Status::ERR_GUEST_MEMORY_ALLOC_FAILED);
    // A failed load must return the guest allocator to empty (no fragmentation).
    ASSERT_EQ(this->getGuestOffset(), static_cast<FwSizeType>(0));
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadBadImportNameFails) {
    StagedAsset file_asset(*this, "bad_import_name.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 16, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // Importing fprime_v1.does_not_exist fails link with ERR_FUNCTION_IMPORT_NOT_FOUND.
    ASSERT_EVENTS_ModuleLoadFailed(0, WasmSequencer_Status::ERR_FUNCTION_IMPORT_NOT_FOUND);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadBadImportModuleFails) {
    StagedAsset file_asset(*this, "bad_import_module.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 17, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // Importing from module "fprime" (host is "fprime_v1") fails link with
    // ERR_FUNCTION_IMPORT_NOT_FOUND.
    ASSERT_EVENTS_ModuleLoadFailed(0, WasmSequencer_Status::ERR_FUNCTION_IMPORT_NOT_FOUND);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadBadImportSigFails) {
    StagedAsset file_asset(*this, "bad_import_sig.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 18, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    // Importing fprime_v1.event with the wrong signature fails link with
    // ERR_FUNCTION_IMPORT_TYPE_MISMATCH.
    ASSERT_EVENTS_ModuleLoadFailed(0, WasmSequencer_Status::ERR_FUNCTION_IMPORT_TYPE_MISMATCH);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// RUN (nominal completion, start functions, traps)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RunEmptyNoBlock) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    // NO_BLOCK responds OK once the module loads; the program then runs to
    // completion and lands in READY.
    this->sendCmd_RUN(0, 20, file, NO_BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 20, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunEmptyBlock) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    // BLOCK holds the command response until the interpreter finishes.
    this->sendCmd_RUN(0, 21, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 21, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunStartModule) {
    // Module with a `start` function that runs at instantiation.
    StagedAsset file_asset(*this, "start.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 22, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 22, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunNoMainFailsInvoke) {
    // Valid module that exports `other` but not `main`.
    StagedAsset file_asset(*this, "no_main.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 23, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_InvalidModuleEntrypoint_SIZE(1);
    // No `main` export resolves to ERR_NOT_FOUND.
    ASSERT_EVENTS_InvalidModuleEntrypoint(0, 0, WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 23, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunMainInvokeFails) {
    // A module whose `main` declares more locals than fit the guest stack. main
    // has a valid [] -> [] signature so moduleHasValidMain passes, but
    // spacewasm_invoke fails at call setup (StackOverflow), taking the
    // MAIN_INVOKE_CHECK failure branch: reportModuleMainInvokeFailed +
    // EXECUTION_ERROR responses -> IDLE. This is the main-invoke analogue of
    // RunStartOverflowTrapsToIdle.
    StagedAsset file_asset(*this, "main_overflow.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 40, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleMainInvokeFailed_SIZE(1);
    // spacewasm_invoke traps at call setup with ERR_STACK_OVERFLOW.
    ASSERT_EVENTS_ModuleMainInvokeFailed(0, WasmSequencer_Status::ERR_STACK_OVERFLOW);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 40, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunMainReturningNonZeroFails) {
    // main has the [] -> i32 signature (the other form validateModuleMain
    // accepts) and returns a non-zero value (42). A non-zero return value is a
    // sequence failure, reported via SequenceExited carrying the returned code --
    // the same disposition as fprime.exit(code != 0). The run lands back in IDLE.
    StagedAsset file_asset(*this, "main_i32.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 42, file, BLOCK, {});
    // The run starts and ends in IDLE, so dispatchUntilControllerState(IDLE) would
    // be a no-op (already there); drain the queue instead.
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceExited_SIZE(1);
    ASSERT_EVENTS_SequenceExited(0, 0, WasmSequencer_SequencePhase::MAIN, 42);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 42, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunUnreachableTraps) {
    StagedAsset file_asset(*this, "unreachable.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 24, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::UNREACHABLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 24, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunDivZeroTraps) {
    StagedAsset file_asset(*this, "divzero.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 25, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::DIVIDE_BY_ZERO);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunExitNonZeroFails) {
    // exit.wasm calls fprime_v1.exit(1). A non-zero exit is a program failure,
    // surfaced as a ProgramExited event (not a trap) with an EXECUTION_ERROR.
    StagedAsset file_asset(*this, "exit.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 26, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceExited(0, 0, WasmSequencer_SequencePhase::MAIN, 1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 26, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);

    // Pin the SequencesFailed increment on the HOST_EXIT (non-zero-exit) branch:
    // assertSequenceFailureCount above only counts the event, not the counter.
    this->flushTelemetry();
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(0));
}

TEST_F(WasmSequencerTester, RunPanicFails) {
    // panic.wasm calls fprime_v1.panic(7). A panic is a program failure, surfaced
    // as a PanicOccurred event (not a trap).
    StagedAsset file_asset(*this, "panic.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 27, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequencePanic(0, 0, WasmSequencer_SequencePhase::MAIN, 7);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 27, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunExitZeroSucceeds) {
    // exit0.wasm calls fprime_v1.exit(0). A zero exit code is a clean success:
    // no trap, no ProgramExited event, and an OK response.
    StagedAsset file_asset(*this, "exit0.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 29, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 29, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunStartTrapsToIdle) {
    // A module whose `start` function contains `unreachable`. The interpreter
    // begins the start function (startInvoked -> RUNNING) and traps while
    // spinning, surfacing as a SequenceTrap and returning to IDLE with an
    // EXECUTION_ERROR response.
    StagedAsset file_asset(*this, "start_trap.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 28, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::START, WasmSequencer_TrapReason::UNREACHABLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 28, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);

    // Regression for the SequencesFailed double-count on the RUN-with-start path
    // (RUNNING_START_PENDING_MAIN): the start-phase failure counts exactly once.
    this->flushTelemetry();
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(0));
}

TEST_F(WasmSequencerTester, RunStartOverflowTrapsToIdle) {
    // A module whose `start` function declares more locals than fit the guest
    // stack. spacewasm_invoke_start fails at call setup (StackOverflow) and
    // returns SPACEWASM_RUN_TRAP *directly* -- exercising the startError branch
    // (STARTING -> invokeStartOfLastModule -> startError -> reportInvokeFailure).
    // This is distinct from start_trap.wasm, whose start begins running
    // (RUN_OUT_OF_FUEL -> startInvoked -> RUNNING) and only traps while spinning.
    StagedAsset file_asset(*this, "start_overflow.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 29, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // reportModuleStartInvokeFailed fires; the exact status is not asserted
    // because m_invokeStatus is not set on the start-invoke path.
    ASSERT_EVENTS_ModuleStartInvokeFailed_SIZE(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 29, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);

    // This start-invoke-setup failure counts as one failed sequence (owned by the
    // countSequenceFailure action, since respond_ERROR no longer counts).
    this->flushTelemetry();
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(0));
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
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    // Load (unnamed) then invoke "main" of the unnamed module.
    this->sendCmd_LOAD(0, 30, file, Fw::CmdStringArg(""));
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
}

TEST_F(WasmSequencerTester, InvokeTrapAfterExitZeroIsNotMisreported) {
    // Regression: exit(0) returns the component to READY without resetting the
    // store, leaving the host exit disposition (HOST_EXIT/0) stale. A subsequent
    // invoke that genuinely traps must reset that disposition per-invoke so the
    // trap is reported as a trap, not misread as a clean completion.
    StagedAsset file_asset(*this, "exit0_then_trap.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 37, file, Fw::CmdStringArg(""));
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
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::UNREACHABLE);
    // Still exactly one success (from the first invoke); the trap did not add one.
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(2, OPCODE_INVOKE, 39, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, InvokeNoBlockRespondsImmediately) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 35, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    // NO_BLOCK INVOKE responds OK as soon as the module resolves, before the
    // function actually runs (INVOKE_cmdHandler NO_BLOCK path).
    this->sendCmd_INVOKE(0, 36, Fw::CmdStringArg(""), NO_BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 36, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, InvokeUnknownModule) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 32, file, Fw::CmdStringArg(""));
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
}

TEST_F(WasmSequencerTester, InvokeFromIdleInvalid) {
    // INVOKE is only valid from READY. From IDLE the controller rejects it as BUSY
    // (ControllerBusy for the COMMAND_INVOKE signal in the IDLE state).
    this->sendCmd_INVOKE(0, 34, Fw::CmdStringArg(""), NO_BLOCK, {});
    this->dispatchAll();

    ASSERT_EVENTS_ControllerCannotInvoke_SIZE(1);
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
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 41, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_WAIT(0, 42);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(1, OPCODE_WAIT, 42, Fw::CmdResponse::OK);
    // WAIT from READY responds immediately without changing state.
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, WaitDuringLoadRespondsOnLoadComplete) {
    // A WAIT that queues while a LOAD is in flight must be answered when the load settles to
    // READY. respond_noblock_OK is the LOAD's only completion action -- a LOAD never runs
    // main, so respond_block_OK/respond_block_ERROR never fire for it -- so it must drain
    // m_waiting. Previously it did not, orphaning the WAIT until an unrelated later sequence
    // answered it. start.wasm has a start function, so a LOAD parks in RUNNING_START (running
    // that start function): a busy state in which a WAIT queues rather than answering now.
    StagedAsset file_asset(*this, "start.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 60, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::RUNNING_START);
    ASSERT_EQ(this->controllerState(), ControllerState::RUNNING_START);

    // Controller is busy (RUNNING_START), so WAIT queues into m_waiting.
    this->sendCmd_WAIT(0, 61);

    // The start function finishes and the load settles to READY, which must answer both the
    // NO_BLOCK LOAD and the queued WAIT with OK.
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 60, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_WAIT, 61, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// Invalid-state command rejections
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ContinueFromIdleInvalid) {
    this->sendCmd_CONTINUE(0, 50);
    this->dispatchAll();
    ASSERT_EVENTS_SequenceNotRunning_SIZE(1);
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
    ASSERT_EQ(mapTrapReason(SPACEWASM_TRAP_UNINITIALIZED_TABLE_ELEMENT), TR::UNINITIALIZED_TABLE_ELEMENT);
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
    StagedAsset file_asset(*this, "event.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 60, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_LogActivityHi_SIZE(1);
    ASSERT_EVENTS_LogActivityHi(0, "hello wasm");
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, EventAllSeverities) {
    StagedAsset file_asset(*this, "event_all_sev.wasm");
    const Fw::String& file = file_asset.file();

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
}

TEST_F(WasmSequencerTester, EventMessageTruncatedToMax) {
    // The guest passes a 250-byte message length; the host clamps it to
    // FW_LOG_STRING_MAX_SIZE (200) before reading (wasmEvent len clamp). The
    // emitted event string is exactly 200 'A' characters.
    StagedAsset file_asset(*this, "event_toobig.wasm");
    const Fw::String& file = file_asset.file();

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
}

TEST_F(WasmSequencerTester, EventBadSeverityReported) {
    StagedAsset file_asset(*this, "event_bad_sev.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 62, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    // The guest requested severity id 99, which is not a valid Fw::LogSeverity.
    // Rather than trap, the host reports it (with the guest message) and lets the
    // guest continue, so the sequence runs to completion.
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_HostFunctionInvalidSeverity_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidSeverity(0, 99, "x");
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// Host functions: TELEMETRY
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, TelemetryRead) {
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    StagedAsset file_asset(*this, "tlm.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 70, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

TEST_F(WasmSequencerTester, TelemetryReadPortNotConnectedTraps) {
    // getTlmChan is a plain (not-required) output port. With it disconnected, tlm()
    // must not invoke the generated invoker (which would FW_ASSERT); instead it logs
    // HostFunctionInvalidPort(TELEMETRY) and traps the sequence.
    this->disconnectGetTlmChan(0);

    StagedAsset file_asset(*this, "tlm.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 74, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPort_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPort(0, WasmSequencer_HostFunction::TELEMETRY, 0, 0);
    this->assertSequenceFailureCount(1);
    // The port was never invoked.
    ASSERT_from_getTlmChan_SIZE(0);
}

TEST_F(WasmSequencerTester, ParameterReadPortNotConnectedTraps) {
    // getParam disconnected: prm() logs HostFunctionInvalidPort(PARAMETER) and traps.
    this->disconnectGetParam(0);

    StagedAsset file_asset(*this, "prm.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 75, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPort_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPort(0, WasmSequencer_HostFunction::PARAMETER, 0, 0);
    this->assertSequenceFailureCount(1);
    ASSERT_from_getParam_SIZE(0);
}

TEST_F(WasmSequencerTester, CommandPortNotConnectedTraps) {
    // cmdOut disconnected: cmd() logs HostFunctionInvalidPort(COMMAND) and traps.
    this->disconnectCmdOut(0);

    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 76, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPort_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPort(0, WasmSequencer_HostFunction::COMMAND, 0, 0);
    this->assertSequenceFailureCount(1);
    ASSERT_from_cmdOut_SIZE(0);
}

TEST_F(WasmSequencerTester, TelemetryReadValueMismatchTraps) {
    U8 raw[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    StagedAsset file_asset(*this, "tlm.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 73, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::UNREACHABLE);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

TEST_F(WasmSequencerTester, TelemetryBadTimeSizeTraps) {
    StagedAsset file_asset(*this, "tlm_badtime.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 71, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // A time_size of 8 < Fw::Time::SERIALIZED_SIZE cannot hold the serialized time and
    // is rejected as too small; the reported valueSize is the required serialized size.
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::TELEMETRY, 8, Fw::Time::SERIALIZED_SIZE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::HOST);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, TelemetryBadTimeSizeTooLargeTraps) {
    StagedAsset file_asset(*this, "tlm_bigtime.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 74, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // A time_size of 16 > Fw::Time::SERIALIZED_SIZE must match exactly and is
    // rejected as too large; the reported maxSize is the required serialized size.
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::TELEMETRY, 16, Fw::Time::SERIALIZED_SIZE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::HOST);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, TelemetryOversizedRequestWritesOnlyValueBytes) {
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    StagedAsset file_asset(*this, "tlm_largebuf.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 72, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_BufferTooLarge_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

TEST_F(WasmSequencerTester, TelemetryUndersizedRequestFails) {
    // A value_size (2) smaller than the actual serialized value (4) is rejected at
    // dispatch with BufferTooSmall; nothing is written into guest memory.
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    StagedAsset file_asset(*this, "tlm_toosmall.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 74, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    // value_size 2 smaller than the 4-byte serialized value.
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::TELEMETRY, 2, static_cast<U32>(sizeof raw));
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

// ----------------------------------------------------------------------
// Host functions: PARAMETER
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ParameterRead) {
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    StagedAsset file_asset(*this, "prm.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 80, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

TEST_F(WasmSequencerTester, ParameterReadValueMismatchTraps) {
    U8 raw[4] = {0x00, 0x11, 0x22, 0x33};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    StagedAsset file_asset(*this, "prm.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 82, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::UNREACHABLE);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

TEST_F(WasmSequencerTester, ParameterOversizedRequestWritesOnlyValueBytes) {
    // Mirror of TelemetryOversizedRequestWritesOnlyValueBytes: an oversized len (64)
    // for a 4-byte value writes only the real value bytes. prm_toobig poisons the byte
    // past the value and traps unless it survives, so success proves no stack leak.
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    StagedAsset file_asset(*this, "prm_largebuf.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 81, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_BufferTooLarge_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

TEST_F(WasmSequencerTester, ParameterUndersizedRequestFails) {
    // A len (2) smaller than the actual serialized value (4) is rejected at dispatch
    // with BufferTooSmall; nothing is written into guest memory.
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    StagedAsset file_asset(*this, "prm_toosmall.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 83, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::PARAMETER, 2, static_cast<U32>(sizeof raw));
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
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

    StagedAsset file_asset(*this, "args.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 200, file, BLOCK, args);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, ArgsRoundTripMismatchTraps) {
    // Negative control: inject a different pattern than args.wasm hard-codes. The guest
    // reads the args back, sees the mismatch, and traps (UNREACHABLE). This proves the
    // round-trip check in ArgsRoundTrip is real: if the host dropped the args, this test
    // would (wrongly) succeed too.
    const U8 argBytes[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    StagedAsset file_asset(*this, "args.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 201, file, BLOCK, args);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::UNREACHABLE);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, ArgsEmpty) {
    // No arguments supplied (default-constructed SeqArgs, size 0). args_empty.wasm
    // asserts the returned count is 0 and that nothing was written to guest memory.
    StagedAsset file_asset(*this, "args_empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 202, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, ArgsUndersizedBufferFails) {
    // The guest declares a 2-byte buffer but 4 arg bytes are present. Writing them would
    // overrun the guest's intent, so the host rejects it at dispatch with
    // BufferTooSmall(ARGS, 2, 4) -> SequenceTrapped, writing nothing to guest memory.
    const U8 argBytes[4] = {0xCA, 0xFE, 0xBA, 0xBE};
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    StagedAsset file_asset(*this, "args_toosmall.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 203, file, BLOCK, args);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::ARGS, 2, static_cast<U32>(sizeof argBytes));
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, ArgsBadPointerFails) {
    // The guest declares an ample buffer (passes the too-small guard) but points args()
    // at an out-of-bounds address. The mem_write fails ->
    // HostFunctionInvalidPointer(ARGS) -> SequenceTrapped.
    const U8 argBytes[4] = {0xCA, 0xFE, 0xBA, 0xBE};
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    StagedAsset file_asset(*this, "args_badptr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 204, file, BLOCK, args);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::ARGS,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, ArgsMixedStructRoundTrip) {
    // Fill the argument buffer to its maximum (SequenceArgumentsMaxSize) with a mixed,
    // packed little-endian struct and validate every field round-trips. This exercises
    // the host->guest arg copy at the full buffer size across differently-sized fields:
    //   U32 0x11223344, U16 0x5566, U8 0x77, U8 0x88, U32 0x99AABBCC  (12 bytes total).
    // args_mixed.wasm reads it back, checks the returned count is 12, verifies each field
    // at its offset, and confirms the byte just past the payload was not written.
    const U8 argBytes[] = {
        0x44, 0x33, 0x22, 0x11,  // U32 a = 0x11223344 (LE)
        0x66, 0x55,              // U16 b = 0x5566 (LE)
        0x77,                    // U8  c
        0x88,                    // U8  d
        0xCC, 0xBB, 0xAA, 0x99,  // U32 e = 0x99AABBCC (LE)
    };
    static_assert(sizeof argBytes == SequenceArgumentsMaxSize,
                  "ArgsMixedStructRoundTrip is meant to fill the argument buffer exactly; "
                  "update the payload if SequenceArgumentsMaxSize changes");
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    StagedAsset file_asset(*this, "args_mixed.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 205, file, BLOCK, args);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, ArgsOversizedSizeRejected) {
    // Regression for a host-memory out-of-bounds read (CWE-125). SeqArgs carries a
    // ground-controlled $size next to a fixed SequenceArgumentsMaxSize (12) byte buffer, and
    // $size is deserialized without clamping. A crafted RUN whose $size exceeds the buffer
    // capacity must be rejected at dispatch: the host must never copy more than the buffer
    // holds, or it leaks adjacent host memory into guest linear memory. args.wasm declares a
    // 64-byte guest buffer, so a claimed size of 32 (>12 capacity, <=64 guest len) slips past
    // the destination-too-small guard and, absent the source-capacity guard, over-reads.
    U8 argBytes[SequenceArgumentsMaxSize];
    for (FwSizeType i = 0; i < sizeof argBytes; i++) {
        argBytes[i] = static_cast<U8>(0xA0 + i);
    }
    Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);
    args.set_size(32);  // claim 32 arg bytes though only 12 physically exist

    StagedAsset file_asset(*this, "args.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 206, file, BLOCK, args);
    this->dispatchAll();

    // Rejected before any guest write: BufferTooLarge(ARGS, requested=32, capacity=12), the
    // sequence fails, and the guest never resumes (nothing leaked, no SequenceSucceeded).
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::ARGS, 32, static_cast<U32>(SequenceArgumentsMaxSize));
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// Time host function (fprime.time)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, TimeRead) {
    // The guest calls time(0, SERIALIZED_SIZE) into a valid buffer. The host
    // getTime()s the injected test time, serializes it into guest memory, and
    // resumes. A clean SequenceSucceeded proves the TIME host function round trip.
    this->setTestTime(Fw::Time(1000, 2000));

    StagedAsset file_asset(*this, "time.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 210, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    this->assertSequenceFailureCount(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, TimeBadPointerFails) {
    // The guest requests a valid time_size but points time() at an out-of-bounds
    // address. The size guards pass; the service block's mem_write then fails ->
    // HostFunctionInvalidPointer(TIME) -> SequenceTrapped.
    StagedAsset file_asset(*this, "time_badptr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 211, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::TIME,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, TimeBadSizeTraps) {
    // The guest requests a time_size of 8 < Fw::Time::SERIALIZED_SIZE (11), which
    // cannot hold the serialized time. wasmTime rejects it up front with
    // BufferTooSmall(TIME, 8, 11) -> TRAP (HOST trap reason); nothing is written.
    StagedAsset file_asset(*this, "time_toosmall.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 212, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::TIME, 8, Fw::Time::SERIALIZED_SIZE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::HOST);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, TimeBadSizeTooLargeTraps) {
    // The guest requests a time_size of 16 > Fw::Time::SERIALIZED_SIZE (11), which
    // must match exactly. wasmTime rejects it up front with
    // BufferTooLarge(TIME, 16, 11) -> TRAP (HOST trap reason); nothing is written.
    StagedAsset file_asset(*this, "time_toobig.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 213, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::TIME, 16, Fw::Time::SERIALIZED_SIZE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::HOST);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
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

    // All channels are `update on change`: flushing again without any state change
    // must emit nothing the second time.
    this->clearHistory();
    this->flushTelemetry();
    ASSERT_TLM_ControllerState_SIZE(0);
    ASSERT_TLM_InterpreterState_SIZE(0);
    ASSERT_TLM_SequencesSucceeded_SIZE(0);
    ASSERT_TLM_CommandsDispatched_SIZE(0);
    ASSERT_TLM_LastTrapReason_SIZE(0);
    ASSERT_TLM_SeqName_SIZE(0);
}

TEST_F(WasmSequencerTester, TelemetryInterpreterStateReflectsEngine) {
    // The InterpreterState channel must report the *interpreter* state machine's
    // state, which is distinct from ControllerState. Guard against a copy-paste
    // transpose (writing controller_getState() into the InterpreterState channel, or
    // vice versa) by asserting both at a moment they necessarily differ: mid-run the
    // controller is RUNNING_MAIN while the interpreter is RUNNING_SPINNING.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 310, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    this->flushTelemetry();
    ASSERT_TLM_ControllerState(0, ControllerState::RUNNING_MAIN);
    ASSERT_TLM_InterpreterState(0, InterpreterState::RUNNING_SPINNING);

    // Clean up the still-running sequence.
    this->sendCmd_CANCEL(0, 311);
    this->dispatchUntilControllerState(ControllerState::IDLE);
}

TEST_F(WasmSequencerTester, TelemetrySuccessCountAndName) {
    // A completed RUN increments SequencesSucceeded, leaves the component READY, and
    // records the sequence name as the filename stem (empty.wasm -> "empty").
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 300, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_ControllerState(0, ControllerState::READY);
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(0));
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(0));
    ASSERT_TLM_LastTrapReason(0, WasmSequencer_TrapReason::NONE);
    ASSERT_TLM_SeqName(0, "empty");

    // SequenceStarting fires exactly once per RUN and carries the started module's
    // index (module 0 here). Previously this event was only ever asserted _SIZE(0).
    ASSERT_EVENTS_SequenceStarting_SIZE(1);
    ASSERT_EVENTS_SequenceStarting(0, 0);
}

TEST_F(WasmSequencerTester, TelemetrySuccessCountAccumulates) {
    // Counters are cumulative across sequences: two successful runs -> 2.
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
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
}

TEST_F(WasmSequencerTester, TelemetryLoadNameRecordsModuleName) {
    // LOAD records the user-provided module name verbatim (not the filename).
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 303, file, Fw::CmdStringArg("mod"));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_SeqName(0, "mod");
}

TEST_F(WasmSequencerTester, TelemetryFailedCount) {
    // A no-main module fails to invoke -> SequencesFailed increments, others stay 0.
    StagedAsset file_asset(*this, "no_main.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 304, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(0));
    ASSERT_TLM_LastTrapReason(0, WasmSequencer_TrapReason::NONE);
}

TEST_F(WasmSequencerTester, TelemetryTrapRecordsReasonAndFails) {
    // A trap records LastTrapReason and counts as a failed sequence.
    StagedAsset file_asset(*this, "unreachable.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 305, file, BLOCK, {});
    this->dispatchAll();

    this->flushTelemetry();

    ASSERT_TLM_ControllerState(0, ControllerState::IDLE);
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(0));
    ASSERT_TLM_LastTrapReason(0, WasmSequencer_TrapReason::UNREACHABLE);
}

TEST_F(WasmSequencerTester, LastTrapReasonClearedAcrossSequences) {
    // LastTrapReason must reflect only the current sequence. A trap records UNREACHABLE;
    // a subsequent clean run must reset it to NONE (clearExitStatus at RUNNING entry),
    // so stale trap telemetry never leaks from a prior sequence into a later one.
    StagedAsset trap_asset(*this, "unreachable.wasm");
    this->sendCmd_RUN(0, 600, trap_asset.file(), BLOCK, {});
    this->dispatchAll();
    this->flushTelemetry();
    ASSERT_TLM_LastTrapReason(0, WasmSequencer_TrapReason::UNREACHABLE);

    // A later successful run clears the recorded trap reason.
    StagedAsset ok_asset(*this, "empty.wasm");
    this->sendCmd_RUN(0, 601, ok_asset.file(), BLOCK, {});
    this->dispatchAll();
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    this->flushTelemetry();
    ASSERT_TLM_LastTrapReason(0, WasmSequencer_TrapReason::NONE);
}

TEST_F(WasmSequencerTester, LastHostFunctionClearedAcrossSequences) {
    // lastHostFunction (reported in SequenceHostFailure) must reflect only the current
    // sequence. Sequence A fails inside the COMMAND host function, leaving
    // lastHostFunction = COMMAND. Sequence B then fails via an unexpected reply while
    // spinning -- it awaited no host function, so it must report NONE, not the stale
    // COMMAND. clearExitStatus resets lastHostFunction at RUNNING entry.
    {
        StagedAsset bad(*this, "cmd_badptr.wasm");
        this->sendCmd_RUN(0, 610, bad.file(), BLOCK, {});
        this->dispatchAll();
        ASSERT_EVENTS_SequenceHostFailure(0, 0, WasmSequencer_SequencePhase::MAIN,
                                          WasmSequencer_ExitReason::HOST_FAILURE, WasmSequencer_HostFunction::COMMAND);
    }
    this->clearHistory();

    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);
    StagedAsset loopmod(*this, "loop.wasm");
    this->sendCmd_RUN(0, 611, loopmod.file(), NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);
    this->invoke_to_cmdResponseIn(0, 0, this->currentCmdUid(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EVENTS_SequenceHostFailure(0, 0, WasmSequencer_SequencePhase::MAIN,
                                      WasmSequencer_ExitReason::UNEXPECTED_REPLY, WasmSequencer_HostFunction::NONE);
}

TEST_F(WasmSequencerTester, TelemetryCancelledCount) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 306, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);
    this->sendCmd_CANCEL(0, 307);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    this->flushTelemetry();

    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(0));
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(0));
}

TEST_F(WasmSequencerTester, TelemetryCommandsDispatchedAndFailed) {
    // cmd.wasm dispatches one command out cmdOut; feeding a non-OK response bumps
    // CommandsFailed while CommandsDispatched counts the dispatch.
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 308, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::EXECUTION_ERROR);
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_CommandsDispatched(0, static_cast<U64>(1));
    ASSERT_TLM_CommandsFailed(0, static_cast<U64>(1));
    // The guest still ran to completion after the failed command response.
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(1));
}

TEST_F(WasmSequencerTester, TelemetryCommandOkDoesNotCountFailed) {
    // An OK command response dispatches but does not increment CommandsFailed.
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 309, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);

    this->flushTelemetry();

    ASSERT_TLM_CommandsDispatched(0, static_cast<U64>(1));
    ASSERT_TLM_CommandsFailed(0, static_cast<U64>(0));
}

// ----------------------------------------------------------------------
// Host functions: COMMAND (byte-fidelity of guest bytes -> cmdOut ComBuffer)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, CommandByteFidelityAndResume) {
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();

    // RUN pauses when the guest calls cmd, dispatches the command out cmdOut,
    // and parks in AWAITING_RESPONSE until we feed a cmdResponseIn.
    this->sendCmd_RUN(0, 90, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
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
}

TEST_F(WasmSequencerTester, CommandTooBigTraps) {
    StagedAsset file_asset(*this, "cmd_toobig.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 91, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    // len 600 + packet descriptor > FW_COM_BUFFER_MAX_SIZE; the reported maxSize
    // is the payload budget after reserving the descriptor prefix.
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::COMMAND, 600,
                                 FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType));
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::HOST);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
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
    StagedAsset file_asset(*this, "cmd_badptr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 92, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    // The OOB buffer read fails with ERR_MEM_OUT_OF_BOUNDS in the COMMAND path.
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::COMMAND,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
    // The guest-memory read failure aborts the sequence as a HOST_FAILURE, naming the
    // COMMAND host function (pins the HOST_FAILURE exit-reason and its payload).
    ASSERT_EVENTS_SequenceHostFailure(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_ExitReason::HOST_FAILURE,
                                      WasmSequencer_HostFunction::COMMAND);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 92, Fw::CmdResponse::EXECUTION_ERROR);
    // No command was actually dispatched.
    ASSERT_from_cmdOut_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, EventBadPointerFails) {
    StagedAsset file_asset(*this, "event_badptr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 93, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::EVENT,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, ParameterBadPointerFails) {
    U8 raw[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->nextPrmValue = Fw::ParamBuffer(raw, sizeof raw);
    this->nextPrmId = 7;

    StagedAsset file_asset(*this, "prm_badptr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 94, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // getParam was queried, but writing the value into guest memory failed.
    ASSERT_from_getParam_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getParam->at(0).id, static_cast<FwPrmIdType>(7));
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::PARAMETER,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

TEST_F(WasmSequencerTester, TelemetryBadTimePointerFails) {
    // The time write (first mem_write) fails on an OOB time pointer.
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    StagedAsset file_asset(*this, "tlm_badtimeptr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 95, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    // The time (first) mem_write fails on the OOB pointer in the TELEMETRY path.
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::TELEMETRY,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

TEST_F(WasmSequencerTester, TelemetryBadValuePointerFails) {
    // The time write succeeds but the value write (second mem_write) fails on an
    // OOB value pointer.
    U8 raw[4] = {0x11, 0x22, 0x33, 0x44};
    this->nextTlmValue = Fw::TlmBuffer(raw, sizeof raw);
    this->nextTlmTime = Fw::Time(1000, 2000);
    this->nextTlmId = 42;

    StagedAsset file_asset(*this, "tlm_badvalptr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 96, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_from_getTlmChan_SIZE(1);
    ASSERT_EQ(this->fromPortHistory_getTlmChan->at(0).id, static_cast<FwChanIdType>(42));
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    // The value (second) mem_write fails on the OOB pointer in the TELEMETRY path.
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::TELEMETRY,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

// ----------------------------------------------------------------------
// Sleep host functions + checkTimers wake path
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RelativeSleepWakes) {
    StagedAsset file_asset(*this, "rsleep.wasm");
    const Fw::String& file = file_asset.file();

    // Start at t=0. rsleep asks for a 1s relative timer.
    this->setTestTime(Fw::Time(0, 0));
    this->sendCmd_RUN(0, 100, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_SLEEPING);

    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());

    // A checkTimers tick before the deadline does not wake it.
    this->setTestTime(Fw::Time(0, 500000));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_SLEEPING);

    // Past the deadline, the timer fires and the interpreter resumes/finishes.
    this->setTestTime(Fw::Time(2, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RelativeSleepDeadlineOverflowDoesNotWakeEarly) {
    // Regression for the relative-sleep deadline overflow: a duration whose whole-seconds
    // part is U32_MAX passes the wasmRsleep gate but, added to a non-zero current time,
    // overflows the U32 seconds field of Fw::Time. The deadline must saturate to the far
    // future instead of wrapping into the past, so the guest stays asleep.
    StagedAsset file_asset(*this, "rsleep_near_max.wasm");
    const Fw::String& file = file_asset.file();

    // Non-zero epoch so now + U32_MAX overflows the U32 seconds field.
    this->setTestTime(Fw::Time(10, 0));
    this->sendCmd_RUN(0, 104, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());
    // The gate must NOT have rejected this sleep; it passes in isolation.
    ASSERT_EVENTS_SleepDurationTooLarge_SIZE(0);

    // A checkTimers tick far in the future but well below U32_MAX seconds must NOT wake the
    // guest. Pre-fix, the wrapped deadline (~9 s) is in the past, so the guest would resume
    // here and the sequence would finish.
    this->setTestTime(Fw::Time(1000000, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());
}

TEST_F(WasmSequencerTester, AbsoluteSleepWakes) {
    StagedAsset file_asset(*this, "asleep.wasm");
    const Fw::String& file = file_asset.file();

    this->setTestTime(Fw::Time(0, 0));
    this->sendCmd_RUN(0, 101, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());

    // asleep target is 10s from epoch.
    this->setTestTime(Fw::Time(20, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, SleepTimeBaseMismatchFails) {
    StagedAsset file_asset(*this, "rsleep.wasm");
    const Fw::String& file = file_asset.file();

    // Set the timer using a specific time base.
    this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 0, 0));
    this->sendCmd_RUN(0, 102, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());

    // Change the time base: comparison becomes INCOMPARABLE -> timeOpFailed.
    this->setTestTime(Fw::Time(TimeBase::TB_SC_TIME, 0, 5, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, SleepDurationOverflowFails) {
    // A guest sleep whose whole-seconds part (micros / 1e6) overflows the U32
    // seconds field of Fw::Time is rejected up front by wasmRsleep with
    // SleepDurationTooLarge -> TRAP (HOST trap reason), rather than silently
    // truncating the timer. The sequence fails to IDLE.
    StagedAsset file_asset(*this, "sleep_overflow.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 103, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SleepDurationTooLarge_SIZE(1);
    ASSERT_EVENTS_SleepDurationTooLarge(0, WasmSequencer_HostFunction::RSLEEP, static_cast<U64>(0x7FFFFFFFFFFFFFFFULL));
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::HOST);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, AbsoluteSleepDurationOverflowFails) {
    // Absolute-sleep analogue of SleepDurationOverflowFails: an asleep() whose
    // whole-seconds part (micros / 1e6) overflows the U32 seconds field of Fw::Time
    // is rejected by wasmAsleep with SleepDurationTooLarge(ASLEEP) -> TRAP; the
    // sequence fails to IDLE. (Exercises the ASLEEP guard distinct from the RSLEEP one.)
    StagedAsset file_asset(*this, "asleep_overflow.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 104, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SleepDurationTooLarge_SIZE(1);
    ASSERT_EVENTS_SleepDurationTooLarge(0, WasmSequencer_HostFunction::ASLEEP, static_cast<U64>(0x7FFFFFFFFFFFFFFFULL));
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceTrapped(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_TrapReason::HOST);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// PAUSE / CONTINUE / CANCEL across the run state machine (loop.wasm)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, PauseThenContinueCompletes) {
    // Tiny fuel makes the busy-loop span many OUT_OF_FUEL cycles so PAUSE can
    // land between host-function dispatches deterministically.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 110, file, NO_BLOCK, {});
    // Advance into the running loop.
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    // Request a pause; the machine pauses before the next spin.
    this->sendCmd_PAUSE(0, 111);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_PAUSED);
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_PAUSED);
    ASSERT_EVENTS_SequencePaused_SIZE(1);

    // Continue: it resumes spinning and eventually finishes.
    this->sendCmd_CONTINUE(0, 112);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    // CONTINUE from PAUSED answers OK (RUN OK index 0, PAUSE OK index 1, CONTINUE index 2).
    ASSERT_CMD_RESPONSE(2, OPCODE_CONTINUE, 112, Fw::CmdResponse::OK);
    // Value-assert the success event's module index (not just its presence).
    ASSERT_EVENTS_SequenceSucceeded(0, 0);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, CancelWhileSpinning) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 120, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    // CANCEL is synchronous (responds OK immediately) and returns to IDLE.
    this->sendCmd_CANCEL(0, 121);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // Value-assert the cancel event's module index and phase (cancelled while in main).
    ASSERT_EVENTS_SequenceCancelled(0, 0, WasmSequencer_SequencePhase::MAIN);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, CancelWhilePaused) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 122, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    this->sendCmd_PAUSE(0, 123);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_PAUSED);

    this->sendCmd_CANCEL(0, 124);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, CancelWhileAwaitingResponse) {
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 125, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    // CANCEL from AWAITING_RESPONSE clears the pending host function and returns
    // to IDLE.
    this->sendCmd_CANCEL(0, 126);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(1);
}

TEST_F(WasmSequencerTester, CancelFromReadyReturnsIdle) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 130, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_CANCEL(0, 131);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
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

// ----------------------------------------------------------------------
// CANCEL during the load/start window (before the engine runs)
//
// A CANCEL that lands while the controller is still loading/resolving (engine
// not yet RUNNING) used to be dropped by both state machines while the operator
// still received OK, and the sequence ran anyway. The deferred-cancel latch now
// records such a cancel and honors it at the next decision point: the sequence
// does not run, the pending request is answered EXECUTION_ERROR, and CANCEL's OK
// becomes truthful. Ordering note: enqueuing the request and CANCEL back-to-back
// makes the FIFO record the cancel before the queued loadSucceeded/invoked, so
// the divert fires deterministically.
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RunCancelledDuringLoadDiverts) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    // RUN is async (a queued command); CANCEL is sync (its handler runs immediately).
    // Dispatch the RUN command first so the `run` signal is queued and the controller
    // begins loading; the CANCEL then latches while the controller is LOADING_TO_RUN,
    // ahead of the queued loadSucceeded, so the post-load cancel-check diverts.
    this->sendCmd_RUN(0, 140, file, BLOCK, {});
    this->dispatchOne();
    this->sendCmd_CANCEL(0, 141);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // CANCEL replies OK immediately; the RUN is answered EXECUTION_ERROR at the divert.
    ASSERT_CMD_RESPONSE(0, OPCODE_CANCEL, 141, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_RUN, 140, Fw::CmdResponse::EXECUTION_ERROR);
    // The sequence never began executing.
    ASSERT_EVENTS_SequenceStarting_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    this->flushTelemetry();
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(0));
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(0));
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadCancelledDuringLoadDiverts) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 142, file, Fw::CmdStringArg(""));
    this->dispatchOne();  // dispatch async LOAD so the load signal is queued before sync CANCEL
    this->sendCmd_CANCEL(0, 143);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_CANCEL, 143, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_LOAD, 142, Fw::CmdResponse::EXECUTION_ERROR);
    this->flushTelemetry();
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(0));
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, InvokeCancelledDuringInvokingDiverts) {
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    // Load a module so INVOKE has something to resolve, landing in READY.
    this->sendCmd_LOAD(0, 144, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    // INVOKE then CANCEL back-to-back: the cancel is latched in INVOKING and honored
    // at the post-resolve cancel-check, before main runs.
    this->sendCmd_INVOKE(0, 145, Fw::CmdStringArg(""), BLOCK, {});
    this->dispatchOne();  // dispatch async INVOKE so the invoke signal is queued before sync CANCEL
    this->sendCmd_CANCEL(0, 146);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 144, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_CANCEL, 146, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(2, OPCODE_INVOKE, 145, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EVENTS_SequenceStarting_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    this->flushTelemetry();
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(1));
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, RunWithStartCancelledInStartMainGapDiverts) {
    // start.wasm has both a start function and a main. A CANCEL latched after the
    // start finishes but before main is invoked (the interpreter-idle start->main gap)
    // must prevent main from running. Regression guard for the RUNNING_START_PENDING_MAIN
    // cancel-check.
    StagedAsset file_asset(*this, "start.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 147, file, BLOCK, {});
    // Controller waits in RUNNING_START_PENDING_MAIN; drive the interpreter to spin the
    // start (entry `entered` still queued), then run that spin so the (empty) start
    // finishes -- interpreterFinished is now queued and the interpreter is about to
    // return to IDLE. A cancel injected here is dropped by the idle interpreter, so the
    // RUNNING_START_PENDING_MAIN latch + START_MAIN_CANCEL_CHECK is the only thing that
    // can keep main from running.
    this->dispatchUntilControllerState(ControllerState::RUNNING_START_PENDING_MAIN);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);
    this->dispatchOne();  // run the start spin -> interpreterFinished queued

    this->sendCmd_CANCEL(0, 148);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_CANCEL, 148, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_RUN, 147, Fw::CmdResponse::EXECUTION_ERROR);
    // main never started or completed.
    ASSERT_EVENTS_SequenceStarting_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    this->flushTelemetry();
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(0));
    // A cancel during the start->main window is counted as cancelled only, never failed.
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(0));
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, CancelAfterEngineRunningCountsOnce) {
    // A CANCEL after the engine is already RUNNING is handled by the interpreter path
    // (not the load-window latch); it must still count exactly once.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 149, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    this->sendCmd_CANCEL(0, 150);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    this->flushTelemetry();
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(1));
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, CancelLatchDoesNotLeak) {
    // After a divert, the cancel latch is cleared by the next attempt: a subsequent
    // RUN of a good module runs to success rather than being spuriously diverted.
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    // First: divert a RUN during load.
    this->sendCmd_RUN(0, 151, file, BLOCK, {});
    this->dispatchOne();  // dispatch async RUN before the sync CANCEL so the cancel latches while loading
    this->sendCmd_CANCEL(0, 152);
    this->dispatchAll();
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_CANCEL, 152, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_RUN, 151, Fw::CmdResponse::EXECUTION_ERROR);

    // Second: a fresh RUN with no cancel must run to completion.
    this->sendCmd_RUN(0, 153, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(2, OPCODE_RUN, 153, Fw::CmdResponse::OK);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);

    this->flushTelemetry();
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(1));  // only the first attempt
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(1));  // the second attempt
}

TEST_F(WasmSequencerTester, SeqCancelInDuringLoadDiverts) {
    // The seqCancelIn port latches a cancel during load like the CANCEL command. A
    // port-sourced RUN cancelled before it runs emits no cmdResponse and neither
    // seqStartOut nor seqDoneOut (the pair stays balanced, as on the load-failure path).
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    this->invoke_to_seqRunIn(0, file, Svc::SeqArgs());
    this->invoke_to_seqCancelIn(0);
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceStarting_SIZE(0);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(0);
    ASSERT_EQ(this->seqStartOutCount, 0u);
    ASSERT_EQ(this->seqDoneOutCount, 0u);
    this->flushTelemetry();
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(1));
}

TEST_F(WasmSequencerTester, CancelDuringFailedInvokeDoesNotLeakToNextInvoke) {
    // A cancel latched during an invoke that then FAILS to resolve returns the
    // controller to READY (not IDLE), bypassing the cancel-check. Entering READY must
    // acknowledge/clear the latch, so a subsequent uncancelled INVOKE runs normally
    // instead of being spuriously diverted. Guards against narrowing the clear to
    // IDLE-only (which would leak the latch across the invokeFailed -> READY edge).
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_LOAD(0, 154, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    // INVOKE an unknown module (async) then CANCEL (sync) latched during INVOKING; the
    // invoke fails to resolve -> READY, so only READY's entry clears the latch.
    this->sendCmd_INVOKE(0, 155, Fw::CmdStringArg("nope"), BLOCK, {});
    this->dispatchOne();
    this->sendCmd_CANCEL(0, 156);
    this->dispatchAll();
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_ModuleNotFound_SIZE(1);

    // A fresh, uncancelled INVOKE of the loaded module must run to success (not diverted).
    this->sendCmd_INVOKE(0, 157, Fw::CmdStringArg(""), BLOCK, {});
    this->dispatchAll();
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);

    this->flushTelemetry();
    // The failed invoke counts as failed; the second invoke succeeds; nothing was
    // actually cancelled (the latched cancel was acknowledged at READY, not consumed).
    ASSERT_TLM_SequencesSucceeded(0, static_cast<U64>(1));
    ASSERT_TLM_SequencesCancelled(0, static_cast<U64>(0));
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, UnexpectedCmdResponseWhilePausedFails) {
    // A cmdResponseIn while RUNNING_PAUSED (not awaiting a host command) is unexpected
    // and fails the sequence (stmtUnexpected from RUNNING_PAUSED -> IDLE).
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 133, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    this->sendCmd_PAUSE(0, 134);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_PAUSED);

    // Tag the response with the current sequence's cmdUid so it isn't dismissed as
    // a late reply from an old sequence; it is unexpected here and fails.
    this->invoke_to_cmdResponseIn(0, 0, this->currentCmdUid(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// seqRunIn / seqCancelIn ports and seqStartOut / seqDoneOut reporting
// (mirrors the FpySequencer port contract)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, SeqStartDoneEmittedOnRunCommandSuccess) {
    // A RUN command that runs to completion reports a start (echoing the file and
    // args) and then a done with OK to internal callers.
    const U8 argBytes[] = {0xDE, 0xAD, 0xBE, 0xEF};
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 200, file, NO_BLOCK, args);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 200, Fw::CmdResponse::OK);

    // Exactly one start (with the run's file + args) and one done with OK.
    ASSERT_EQ(this->seqStartOutCount, 1u);
    ASSERT_EQ(this->lastSeqStartFilename, file);
    ASSERT_EQ(this->lastSeqStartArgs, args);
    ASSERT_EQ(this->seqDoneOutCount, 1u);
    ASSERT_EQ(this->lastSeqDoneResponse, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, SeqDoneEmittedWithErrorOnRunFailure) {
    // A RUN whose module traps reports a start, then a done with EXECUTION_ERROR.
    StagedAsset file_asset(*this, "unreachable.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 201, file, BLOCK, {});
    // The run both starts and ends in IDLE (it traps), so pump the whole queue
    // rather than dispatching "until IDLE" (which would be an immediate no-op).
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 201, Fw::CmdResponse::EXECUTION_ERROR);

    ASSERT_EQ(this->seqStartOutCount, 1u);
    ASSERT_EQ(this->seqDoneOutCount, 1u);
    ASSERT_EQ(this->lastSeqDoneResponse, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, SeqDoneEmittedWithErrorOnRunCancel) {
    // Cancelling a running RUN reports the start, then a done with EXECUTION_ERROR.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 202, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    ASSERT_EQ(this->seqStartOutCount, 1u);
    ASSERT_EQ(this->seqDoneOutCount, 0u);

    this->sendCmd_CANCEL(0, 203);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    ASSERT_EQ(this->seqDoneOutCount, 1u);
    ASSERT_EQ(this->lastSeqDoneResponse, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, InvokeDoesNotEmitSeqStartOrDone) {
    // seqStart/seqDone are RUN-scoped. An INVOKE (which also exercises the engine)
    // must not report a start or done to internal callers.
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 204, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_INVOKE(0, 205, Fw::CmdStringArg(""), BLOCK, {});
    // INVOKE starts and ends in READY; pump the queue rather than waiting for a
    // state change that already holds.
    this->dispatchAll();

    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 205, Fw::CmdResponse::OK);
    ASSERT_EQ(this->seqStartOutCount, 0u);
    ASSERT_EQ(this->seqDoneOutCount, 0u);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, SeqRunInPortRunsSequence) {
    // The seqRunIn port drives a non-blocking RUN: it reports a start, runs to
    // completion in READY, and reports a done with OK. There is no command
    // response because the run was requested from a port, not a command.
    const U8 argBytes[] = {0x01, 0x02, 0x03};
    const Svc::SeqArgs args = this->makeSeqArgs(argBytes, sizeof argBytes);

    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->invoke_to_seqRunIn(0, file, args);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);

    ASSERT_EQ(this->seqStartOutCount, 1u);
    ASSERT_EQ(this->lastSeqStartFilename, file);
    ASSERT_EQ(this->lastSeqStartArgs, args);
    ASSERT_EQ(this->seqDoneOutCount, 1u);
    ASSERT_EQ(this->lastSeqDoneResponse, Fw::CmdResponse::OK);

    // No command was involved: nothing on cmdResponse, no stray external calls.
    ASSERT_CMD_RESPONSE_SIZE(0);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, SeqRunInFromReadyRuns) {
    // seqRunIn is valid from READY (a prior LOAD left a store). It resets the
    // store, loads, and runs the new module.
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 206, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->invoke_to_seqRunIn(0, file, Svc::SeqArgs());
    // A RUN from READY resets the store, loads, runs, and settles back in READY;
    // pump the queue rather than waiting for a state change that already holds.
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EQ(this->seqStartOutCount, 1u);
    ASSERT_EQ(this->seqDoneOutCount, 1u);
    ASSERT_EQ(this->lastSeqDoneResponse, Fw::CmdResponse::OK);
}

TEST_F(WasmSequencerTester, SeqRunInWhileRunningRejected) {
    // seqRunIn is only valid from IDLE or READY. A run request while already
    // running is rejected as BUSY (ControllerBusy for the PORT_RUN signal in the
    // RUNNING_MAIN state) and leaves the run untouched.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 207, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    const U32 startsBefore = this->seqStartOutCount;

    // The rejected call is handled asynchronously; dispatch it.
    this->invoke_to_seqRunIn(0, file, Svc::SeqArgs());
    this->dispatchAll();

    ASSERT_EVENTS_ControllerBusy_SIZE(1);
    ASSERT_EVENTS_ControllerBusy(0, WasmSequencer_SignalSource::PORT_RUN, ControllerState::RUNNING_MAIN);
    // No new start was reported; the original run is still active.
    ASSERT_EQ(this->seqStartOutCount, startsBefore);

    // Clean up: cancel the still-running sequence.
    this->sendCmd_CANCEL(0, 208);
    this->dispatchUntilControllerState(ControllerState::IDLE);
}

TEST_F(WasmSequencerTester, SeqCancelInCancelsRunningSequence) {
    // The seqCancelIn port cancels a running sequence just like the CANCEL command,
    // driving it back to IDLE and reporting a done with EXECUTION_ERROR.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->invoke_to_seqRunIn(0, file, Svc::SeqArgs());
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    this->invoke_to_seqCancelIn(0);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
    ASSERT_EQ(this->seqDoneOutCount, 1u);
    ASSERT_EQ(this->lastSeqDoneResponse, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, PauseAtHostFunctionThenContinueResumes) {
    // cmd.wasm reaches the cmd host function on its very first spin. dispatchUntilState
    // stops at RUNNING_SPINNING with the entry `entered` signal still queued (before that
    // first spin runs), so a PAUSE queued now is processed right after the spin that hits
    // the host function: RUNNING_SPINNING_HOST_FUNCTION sees pendingPause and enters
    // RUNNING_PAUSED with the host function still pending, rather than dispatching it.
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 135, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    this->sendCmd_PAUSE(0, 136);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_PAUSED);
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_PAUSED);
    ASSERT_EVENTS_SequencePaused_SIZE(1);
    // The command has not been dispatched: the pause pre-empted the host-function
    // dispatch, and the pending host function is retained across the pause.
    ASSERT_from_cmdOut_SIZE(0);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::COMMAND);

    // CONTINUE resumes into the pending host function (PAUSED_RESUME host-function
    // branch): the command is dispatched and the machine awaits its response.
    this->sendCmd_CONTINUE(0, 137);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_from_cmdOut_SIZE(1);

    // Feed the response so the sequence finishes cleanly.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_from_cmdOut_SIZE(1);
}

TEST_F(WasmSequencerTester, CheckTimersWhileAwaitingWithoutTimer) {
    // A COMMAND host function parks in AWAITING_RESPONSE with NO pending timer (unlike
    // sleep). A checkTimers tick there takes the CHECK_TIMERS else branch
    // (guard_pendingTimer false -> checkTimeout only) and stays awaiting.
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 138, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_FALSE(this->hasPendingTimer());

    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    // No timer to wake it: it remains awaiting the command response.
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Response still resumes it to completion.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

// ----------------------------------------------------------------------
// Host-function timeout for blocking async host functions
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, HostFunctionTimeoutFailsAwaitingCommand) {
    // With HOST_FUNCTION_TIMEOUT_SECS set, a command that never gets a response times
    // out on a checkTimers tick past the deadline and fails the sequence.
    this->paramSet_HOST_FUNCTION_TIMEOUT_SECS(5.0f, Fw::ParamValid::VALID);
    this->component.loadParameters();  // refresh the component's cached parameter value
    this->setTestTime(Fw::Time(0, 0));

    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 400, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    // A tick before the deadline keeps it awaiting.
    this->setTestTime(Fw::Time(3, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    // A tick past the deadline times the host function out.
    this->setTestTime(Fw::Time(10, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    // A reply timeout aborts as a host failure naming the awaited COMMAND, and counts
    // one failed sequence (pins the REPLY_TIMEOUT exit-reason and the counter).
    ASSERT_EVENTS_SequenceHostFailure(0, 0, WasmSequencer_SequencePhase::MAIN, WasmSequencer_ExitReason::REPLY_TIMEOUT,
                                      WasmSequencer_HostFunction::COMMAND);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 400, Fw::CmdResponse::EXECUTION_ERROR);
    this->flushTelemetry();
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
}

TEST_F(WasmSequencerTester, HostFunctionTimeoutTimeIncomparableFails) {
    // While AWAITING_RESPONSE for a COMMAND (WAITING, not sleeping), a checkTimers
    // tick runs checkTimeout. If the current time is incomparable to the host function
    // start (the time base/context changed), the deadline cannot be evaluated:
    // checkTimeout raises hostResponseTimeIncomparable -> TIMER_INCOMPARABLE exit
    // and the sequence fails to IDLE. This is the host-function-timeout analogue of
    // SleepTimeBaseMismatchFails (which exercises the SLEEPING/checkSleepTimers path).
    this->paramSet_HOST_FUNCTION_TIMEOUT_SECS(5.0f, Fw::ParamValid::VALID);
    this->component.loadParameters();
    this->setTestTime(Fw::Time(TimeBase::TB_WORKSTATION_TIME, 0, 0, 0));

    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 401, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Change the time base: comparison against the host-function-start deadline becomes
    // INCOMPARABLE.
    this->setTestTime(Fw::Time(TimeBase::TB_SC_TIME, 0, 10, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_EVENTS_SequenceHostFailure(0, 0, WasmSequencer_SequencePhase::MAIN,
                                      WasmSequencer_ExitReason::TIMER_INCOMPARABLE,
                                      WasmSequencer_HostFunction::COMMAND);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 401, Fw::CmdResponse::EXECUTION_ERROR);
    // Pins the SequencesFailed increment on the default/host-failure branch.
    this->flushTelemetry();
    ASSERT_TLM_SequencesFailed(0, static_cast<U64>(1));
}

TEST_F(WasmSequencerTester, HostFunctionTimeoutDisabledByZero) {
    // With HOST_FUNCTION_TIMEOUT_SECS explicitly set to 0 (disabled), no amount of elapsed
    // time times out an awaiting command. The shipped default is now 60s, so a test that
    // wants the disabled behavior must set the parameter to 0 rather than rely on the default.
    this->paramSet_HOST_FUNCTION_TIMEOUT_SECS(0.0f, Fw::ParamValid::VALID);
    this->component.loadParameters();
    this->setTestTime(Fw::Time(0, 0));

    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 401, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Advance well past any plausible timeout and tick: still awaiting.
    this->setTestTime(Fw::Time(100000, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchAll();
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    // The response still resumes it cleanly.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

TEST_F(WasmSequencerTester, HostFunctionTimeoutFailsBlockingSerialRecv) {
    // The host-function timeout applies equally to a blocking serial_recv parked waiting for a
    // message that never arrives: dispatchPendingHostFunction arms the host-function timeout clock in the
    // BLOCKING/empty-queue branch, so checkTimeout eventually fails the sequence.
    this->paramSet_HOST_FUNCTION_TIMEOUT_SECS(2.0f, Fw::ParamValid::VALID);
    this->component.loadParameters();  // refresh the component's cached parameter value
    this->setTestTime(Fw::Time(0, 0));

    StagedAsset file_asset(*this, "serial_recv.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 402, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::SERIAL_RECV);

    this->setTestTime(Fw::Time(30, 0));
    this->invoke_to_checkTimers(0, 0);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 402, Fw::CmdResponse::EXECUTION_ERROR);
}

// ----------------------------------------------------------------------
// Command context (cmdUid): late-reply and wrong-instance detection
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, LateCmdResponseFromOldSequenceIgnored) {
    // A response tagged with a previous sequence's cmdUid is a nominal late reply
    // (e.g. after a CANCEL). It is reported, not failed, and does not disturb the
    // sequence currently awaiting its own response.
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 410, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Craft a cmdUid from an older sequence index (current - 1).
    const U32 currentUid = this->currentCmdUid();
    const U16 oldSeq = static_cast<U16>(((currentUid >> 16) & 0xFFFF) - 1);
    const U32 staleUid = static_cast<U32>((static_cast<U32>(oldSeq) << 16) | (currentUid & 0xFFFF));

    this->invoke_to_cmdResponseIn(0, 0, staleUid, Fw::CmdResponse::OK);
    this->dispatchAll();

    // Still awaiting; the stale reply was ignored with a warning.
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
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
}

TEST_F(WasmSequencerTester, WrongCmdResponseIndexFails) {
    // A response from the current sequence but a different command instance (wrong
    // low-half index) is an integrity error and fails the sequence.
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 411, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

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
    this->assertSequenceFailureCount(1);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 411, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(WasmSequencerTester, SequenceIndexIncrementsAcrossRuns) {
    // Regression for the cmdUid sequence-index counter (m_sequencesStarted): each
    // freshly-invoked program must stamp a DISTINCT sequence index into the high half
    // of the cmdUid it attaches to dispatched commands. If the counter never advanced
    // (the original defect) both runs would stamp sequence index 0 and the
    // cross-sequence late-reply detection below would be silently dead. Unlike
    // LateCmdResponseFromOldSequenceIgnored (which synthesizes a stale uid within one
    // run), this drives two real runs so it fails if the counter is stuck.
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();

    // First run: park awaiting its command response and capture the dispatched cmdUid.
    this->sendCmd_RUN(0, 420, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    const U32 firstUid = this->lastCmdContext();
    const U16 firstSeqIdx = static_cast<U16>((firstUid >> 16) & 0xFFFF);

    // Tear the first run down without ever giving it its response.
    this->sendCmd_CANCEL(0, 421);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    // Second run: park again and capture its cmdUid.
    this->sendCmd_RUN(0, 422, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    const U32 secondUid = this->lastCmdContext();
    const U16 secondSeqIdx = static_cast<U16>((secondUid >> 16) & 0xFFFF);

    // The sequence index advanced between runs (would both be 0 with the counter bug),
    // by exactly one for two back-to-back single-window runs.
    ASSERT_NE(firstSeqIdx, secondSeqIdx) << "sequence index did not advance across runs";
    ASSERT_EQ(static_cast<U16>(firstSeqIdx + 1), secondSeqIdx)
        << "expected the sequence index to advance by one (first=" << firstSeqIdx << " second=" << secondSeqIdx << ")";

    // Complete the second run so the component ends cleanly.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

TEST_F(WasmSequencerTester, LateCmdResponseFromPreviousRunIgnored) {
    // End-to-end companion to LateCmdResponseFromOldSequenceIgnored. Rather than
    // synthesizing a stale cmdUid, this runs two real sequences and delivers the
    // ACTUAL cmdUid dispatched by the first (cancelled) run to the live second run.
    // This only classifies as an old-sequence reply because m_sequencesStarted
    // advanced between the runs; with the counter stuck the two runs would share a
    // sequence index and the old reply's low-half command index would instead be
    // treated as a wrong-instance integrity error that FAILS the live sequence.
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();

    // First run parks awaiting its command response; capture its real cmdUid, then
    // cancel it before it is ever answered.
    this->sendCmd_RUN(0, 423, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    const U32 firstRunUid = this->lastCmdContext();

    this->sendCmd_CANCEL(0, 424);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);

    // Second run parks awaiting its own (distinct) command response.
    this->sendCmd_RUN(0, 425, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    const U32 secondRunUid = this->lastCmdContext();
    ASSERT_NE(firstRunUid, secondRunUid);

    // Deliver the FIRST run's now-stale response to the live second run. It is
    // recognized as coming from an old sequence: reported, ignored, and the live
    // sequence keeps awaiting its own response (it must NOT be failed).
    this->invoke_to_cmdResponseIn(0, 0, firstRunUid, Fw::CmdResponse::OK);
    this->dispatchAll();

    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EVENTS_CmdResponseFromOldSequence_SIZE(1);
    ASSERT_EVENTS_CmdResponseFromOldSequence(0, 0, Fw::CmdResponse::OK, static_cast<U16>((firstRunUid >> 16) & 0xFFFF),
                                             static_cast<U16>((secondRunUid >> 16) & 0xFFFF));

    // The second run's own response still completes it normally.
    this->invoke_to_cmdResponseIn(0, 0, secondRunUid, Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

// ----------------------------------------------------------------------
// Commands rejected / queued while a sequence is running
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, RunWhileRunningRejected) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 150, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    // RUN is only valid from IDLE/READY; from RUNNING it is rejected as BUSY
    // (ControllerBusy for the COMMAND_RUN signal) without disturbing the running
    // loop. The original NO_BLOCK RUN already responded OK at load (index 0); the
    // rejected RUN lands as a BUSY (index 1). The loop still finishes.
    this->sendCmd_RUN(0, 151, file, NO_BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_ControllerBusy_SIZE(1);
    ASSERT_EVENTS_ControllerBusy(0, WasmSequencer_SignalSource::COMMAND_RUN,
                                 WasmSequencer_ControllerStateMachine_State::RUNNING_MAIN);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 150, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_RUN, 151, Fw::CmdResponse::BUSY);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadWhileRunningRejected) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 152, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    // LOAD is only valid from IDLE/READY; from RUNNING it is rejected as BUSY
    // (ControllerBusy for the COMMAND_LOAD signal). The original NO_BLOCK RUN
    // responded OK at load (index 0); the rejected LOAD is BUSY (index 1). The
    // loop still finishes.
    this->sendCmd_LOAD(0, 153, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_ControllerBusy_SIZE(1);
    ASSERT_EVENTS_ControllerBusy(0, WasmSequencer_SignalSource::COMMAND_LOAD,
                                 WasmSequencer_ControllerStateMachine_State::RUNNING_MAIN);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 152, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_LOAD, 153, Fw::CmdResponse::BUSY);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, InvokeWhileRunningRejected) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 154, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    // INVOKE is only valid from READY; from a running sequence it is rejected as
    // BUSY (respondInvoke_BUSY -> ControllerBusy for the COMMAND_INVOKE signal in
    // the RUNNING_MAIN state) without disturbing the running loop. This is the
    // distinct busy-state path, versus respondInvoke_ERROR which handles an INVOKE
    // from IDLE (see InvokeFromIdleInvalid). The original NO_BLOCK RUN already
    // responded OK at load (index 0); the rejected INVOKE lands as a BUSY (index 1).
    // The loop still finishes.
    this->sendCmd_INVOKE(0, 155, Fw::CmdStringArg(""), NO_BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_ControllerBusy_SIZE(1);
    ASSERT_EVENTS_ControllerBusy(0, WasmSequencer_SignalSource::COMMAND_INVOKE,
                                 WasmSequencer_ControllerStateMachine_State::RUNNING_MAIN);
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 154, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_INVOKE, 155, Fw::CmdResponse::BUSY);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, WaitWhileRunningQueuesUntilFinish) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 160, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

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
}

TEST_F(WasmSequencerTester, UnexpectedCmdResponseWhileSpinningFails) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 170, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    // A cmdResponseIn while spinning (not awaiting a host command) is "unexpected"
    // and fails the running sequence (stmtUnexpected -> report_seqFailed). Tag it
    // with the current sequence's cmdUid so it is not dismissed as a late reply.
    this->invoke_to_cmdResponseIn(0, 0, this->currentCmdUid(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    // An unexpected reply while spinning aborts as UNEXPECTED_REPLY; no host function
    // was awaited, so the reported host function is NONE (pins the exit-reason payload).
    ASSERT_EVENTS_SequenceHostFailure(0, 0, WasmSequencer_SequencePhase::MAIN,
                                      WasmSequencer_ExitReason::UNEXPECTED_REPLY, WasmSequencer_HostFunction::NONE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, UnexpectedCmdResponseWhileAwaitingSleepFails) {
    // While AWAITING_RESPONSE for a NON-command host function (a sleep), an incoming
    // cmdResponseIn is unexpected: cmdResponseIn_handler sees the pending kind is not
    // COMMAND and raises stmtUnexpected from RUNNING_AWAITING_RESPONSE -> IDLE.
    StagedAsset file_asset(*this, "rsleep.wasm");
    const Fw::String& file = file_asset.file();
    this->setTestTime(Fw::Time(0, 0));
    this->sendCmd_RUN(0, 172, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_SLEEPING);
    ASSERT_TRUE(this->hasPendingTimer());

    // Tag with the current sequence's cmdUid so it reaches the "unexpected" path
    // (the pending host function is a sleep, not a COMMAND) rather than being
    // dismissed as a late reply.
    this->invoke_to_cmdResponseIn(0, 0, this->currentCmdUid(), Fw::CmdResponse::OK);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    this->assertSequenceFailureCount(1);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, LoadWhileReadyReloads) {
    // A LOAD from READY is accepted (cmd_LOAD READY branch: pendLoad + enter LOADING)
    // and reloads the module, ending back in READY.
    StagedAsset file_asset(*this, "empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 173, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_LOAD(0, 174, file, Fw::CmdStringArg(""));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 173, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_LOAD, 174, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, PauseWhileAwaitingResponseIsPending) {
    // PAUSE from RUNNING_AWAITING_RESPONSE records a pending pause (cmd_PAUSE
    // AWAITING_RESPONSE branch: action_pendPause) without leaving the state. When the
    // command response arrives, the pending pause takes effect at PAUSE_CHECK.
    StagedAsset file_asset(*this, "cmd.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 175, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    this->sendCmd_PAUSE(0, 176);
    this->dispatchAll();
    // Still awaiting the response; the pause is only pending.
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);

    // Feeding the response now resolves the pending pause -> RUNNING_PAUSED.
    this->invoke_to_cmdResponseIn(0, 0, this->lastCmdContext(), Fw::CmdResponse::OK);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_PAUSED);
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_PAUSED);
    ASSERT_EVENTS_SequencePaused_SIZE(1);

    // Continue to completion.
    this->sendCmd_CONTINUE(0, 177);
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

TEST_F(WasmSequencerTester, PauseWhilePausedIsIdempotent) {
    // PAUSE from RUNNING_PAUSED is a self-transition that just re-records the pending
    // pause (cmd_PAUSE RUNNING_PAUSED branch: action_pendPause); it stays paused.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 178, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    this->sendCmd_PAUSE(0, 179);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_PAUSED);

    this->sendCmd_PAUSE(0, 180);
    this->dispatchAll();
    // The duplicate PAUSE is a self-transition: still paused, no extra broken event.
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_PAUSED);
    ASSERT_EVENTS_SequencePaused_SIZE(1);

    // CANCEL cleanly tears down the paused sequence.
    this->sendCmd_CANCEL(0, 181);
    this->dispatchUntilControllerState(ControllerState::IDLE);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SequenceCancelled_SIZE(1);
}

TEST_F(WasmSequencerTester, WaitFinishQueueOverflow) {
    // The WAIT queue (m_waiting) is 8 deep. Fill it with WAITs while a sequence is
    // running; the 9th overflows and is rejected with TooManyBlockingCommands +
    // EXECUTION_ERROR (WAIT_cmdHandler enqueue-failure).
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 180, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    // Queue nine WAITs up front (test queue depth is 20) so they are all handled
    // while the loop is still spinning: eight enqueue, the ninth overflows.
    for (U32 i = 0; i < 9; i++) {
        this->sendCmd_WAIT(0, 181 + i);
    }
    this->dispatchUntilControllerState(ControllerState::READY);

    // All queued WAITs are drained on finish and the sequence returns to READY.
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_TooManyBlockingCommands_SIZE(1);
    // The overflowing (9th) WAIT is rejected immediately with EXECUTION_ERROR. It is
    // handled while spinning, so its response lands right after the RUN's load-time OK
    // (index 0) and before the eight enqueued WAITs drain OK on finish.
    ASSERT_CMD_RESPONSE(1, OPCODE_WAIT, 189, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, WaitDrainedWithErrorWhenRunningSequenceCancelled) {
    // A WAIT queued while a sequence runs is answered EXECUTION_ERROR when that
    // sequence is cancelled: respond_block_ERROR drains m_waiting on the main-phase
    // failure path.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 500, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    this->sendCmd_WAIT(0, 501);  // queues: controller busy in RUNNING_MAIN
    this->sendCmd_CANCEL(0, 502);
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // RUN answered OK at load (0); CANCEL answers OK synchronously (1); the queued
    // WAIT is drained EXECUTION_ERROR when the cancel fails the sequence (2).
    ASSERT_CMD_RESPONSE(0, OPCODE_RUN, 500, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_CANCEL, 502, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(2, OPCODE_WAIT, 501, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, WaitDrainedWithErrorWhenRunWithStartCancelled) {
    // A WAIT queued while a RUN-with-start runs its start function is answered
    // EXECUTION_ERROR when a cancel latched in RUNNING_START_PENDING_MAIN diverts the
    // sequence to IDLE at START_MAIN_CANCEL_CHECK: cancelPendingRequest drains m_waiting.
    // The dispatch sequence mirrors RunWithStartCancelledInStartMainGapDiverts so the
    // cancel is honored via the load-window latch (not the running-engine path).
    StagedAsset file_asset(*this, "start.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 510, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::RUNNING_START_PENDING_MAIN);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);
    this->dispatchOne();  // run the (empty) start spin -> interpreterFinished queued

    this->sendCmd_WAIT(0, 511);    // queues: controller still RUNNING_START_PENDING_MAIN
    this->sendCmd_CANCEL(0, 512);  // latched, honored at START_MAIN_CANCEL_CHECK
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // CANCEL answers OK synchronously (0); cancelPendingRequest answers the diverted
    // RUN (1) and the queued WAIT (2) both with EXECUTION_ERROR.
    ASSERT_CMD_RESPONSE(0, OPCODE_CANCEL, 512, Fw::CmdResponse::OK);
    ASSERT_CMD_RESPONSE(1, OPCODE_RUN, 510, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_CMD_RESPONSE(2, OPCODE_WAIT, 511, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, WaitDrainedWithErrorWhenStartFails) {
    // A WAIT queued while a LOAD-with-start runs is answered EXECUTION_ERROR when the
    // start function traps: respond_ERROR drains m_waiting on the start-phase failure.
    StagedAsset file_asset(*this, "start_trap.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 520, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::RUNNING_START);

    this->sendCmd_WAIT(0, 521);  // queues: controller busy running the start function
    this->dispatchUntilControllerState(ControllerState::IDLE);

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    // The failed LOAD (0) and the queued WAIT (1) are both answered EXECUTION_ERROR.
    ASSERT_CMD_RESPONSE(0, OPCODE_LOAD, 520, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_CMD_RESPONSE(1, OPCODE_WAIT, 521, Fw::CmdResponse::EXECUTION_ERROR);
}

// ----------------------------------------------------------------------
// Continue is a no-op while already running
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, ContinueWhileSpinningIsOk) {
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 140, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    this->sendCmd_CONTINUE(0, 141);
    this->dispatchUntilControllerState(ControllerState::READY);
    // CONTINUE while running just responds OK; the loop still finishes.
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    // The RUN (NO_BLOCK) answered OK at load (index 0); CONTINUE answers OK (index 1).
    ASSERT_CMD_RESPONSE(1, OPCODE_CONTINUE, 141, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

// ----------------------------------------------------------------------
// Host functions: SERIAL_OUT (fire-and-forget: guest bytes -> serialOut buffer)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, SerialOutByteFidelityAndResume) {
    StagedAsset file_asset(*this, "serial_out.wasm");
    const Fw::String& file = file_asset.file();

    // The guest calls serial_send on port index 1 with an 8-byte pattern. The host copies
    // the payload out, emits it on serialOut[1], and resumes immediately (fire-and-forget,
    // no reply awaited), so the sequence runs straight to completion.
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
}

TEST_F(WasmSequencerTester, SerialOutInvalidPortTraps) {
    // serial_send with an out-of-range port index is rejected in the host function with a
    // trap before any port invocation.
    StagedAsset file_asset(*this, "serial_out_badport.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 201, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPort_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPort(0, WasmSequencer_HostFunction::SERIAL_OUT, 5, 5);
    this->assertSequenceFailureCount(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
}

TEST_F(WasmSequencerTester, SerialOutPayloadTooLargeTraps) {
    // serial_send with a payload larger than MAX_SERIAL_OUT_SIZE is rejected in the host
    // function with a trap (BufferTooLarge) before any port invocation.
    StagedAsset file_asset(*this, "serial_out_toobig.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 202, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooLarge_SIZE(1);
    ASSERT_EVENTS_BufferTooLarge(0, WasmSequencer_HostFunction::SERIAL_OUT, 300,
                                 Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE);
    this->assertSequenceFailureCount(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
}

TEST_F(WasmSequencerTester, SerialOutBadPointerFails) {
    // serial_send with an out-of-bounds data pointer: spacewasm_mem_read fails ->
    // HostFunctionInvalidPointer(SERIAL_OUT) -> stmtFailure -> IDLE. The port is
    // never invoked.
    StagedAsset file_asset(*this, "serial_out_badptr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 206, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::SERIAL_OUT,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
}

TEST_F(WasmSequencerTester, SerialOutDisconnectedPortTraps) {
    // serial_send to an in-range but DISCONNECTED port is rejected in the host function with a
    // trap (HostFunctionInvalidPort) before any invocation. The guest targets port index 3,
    // which we leave unconnected.
    this->disconnectSerialOut(3);

    StagedAsset file_asset(*this, "serial_out_port3.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 210, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPort_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPort(0, WasmSequencer_HostFunction::SERIAL_OUT, 3, 5);
    this->assertSequenceFailureCount(1);
    ASSERT_EQ(this->serialOutCount, static_cast<U32>(0));
}

TEST_F(WasmSequencerTester, SerialOutSendFailureFailsSequence) {
    // If the connected serial output port reports a serialization failure, the host emits
    // SerialPortSendFailed and fails the sequence. The normal tester InputSerializePort always
    // returns OK, so we re-point serialOut[3] at a stub that returns FW_SERIALIZE_NO_ROOM_LEFT.
    FailingSerialInputPort failingPort;
    this->connectSerialOutTo(3, failingPort);

    StagedAsset file_asset(*this, "serial_out_port3.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 211, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_SerialPortSendFailed_SIZE(1);
    ASSERT_EVENTS_SerialPortSendFailed(0, WasmSequencer_HostFunction::SERIAL_OUT,
                                       static_cast<I32>(Fw::FW_SERIALIZE_NO_ROOM_LEFT));
    this->assertSequenceFailureCount(1);
}

// ----------------------------------------------------------------------
// Host functions: SERIAL_RECV (read from the inbound serialIn queue)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, SerialRecvBlockingDeliversMessageAtDataPtr) {
    // Pre-load a 4-byte message on serialIn[2], then RUN a guest that blocking-receives it.
    // The guest asserts (via unreachable) that the payload lands exactly at data_ptr (not
    // data_ptr+4), that actual_size_ptr holds 4, and that the bytes just before/after are
    // untouched -- a clean SequenceSucceeded proves the round trip with correct offsets.
    const U8 msg[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    this->enqueueSerialIn(2, msg, sizeof msg);

    StagedAsset file_asset(*this, "serial_recv.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 300, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

TEST_F(WasmSequencerTester, SerialRecvBlocksThenWakesOnMessage) {
    // With an empty queue, a blocking serial_recv parks in AWAITING_RESPONSE.WAITING. When a
    // message subsequently arrives on serialIn[2], the serialInMessage signal wakes the
    // engine, the payload is dequeued into guest memory, and the sequence completes.
    StagedAsset file_asset(*this, "serial_recv.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 301, file, BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_AWAITING_RESPONSE_WAITING);
    ASSERT_EQ(this->getPendingHostFunctionKind(), WasmSequencer_HostFunction::SERIAL_RECV);

    // Deliver the message the guest is waiting for.
    const U8 msg[4] = {0xAA, 0xBB, 0xCC, 0xDD};
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE> in(msg, sizeof msg);
    this->invoke_to_serialIn(2, in);
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

TEST_F(WasmSequencerTester, SerialRecvMultiChunkMessage) {
    // A 40-byte message exercises the chunked copy path in dequeueSerialAndResume (one full
    // 32-byte chunk + an 8-byte partial). The guest validates the ramp payload landed at
    // data_ptr with the correct length, proving the multi-chunk copy is byte-accurate.
    U8 msg[40];
    for (FwSizeType i = 0; i < sizeof msg; i++) {
        msg[i] = static_cast<U8>(i + 1);
    }
    this->enqueueSerialIn(2, msg, sizeof msg);

    StagedAsset file_asset(*this, "serial_recv_big.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 302, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

TEST_F(WasmSequencerTester, SerialRecvExactChunkMultipleMessage) {
    // A message whose payload is exactly CHUNK_SIZE (32 bytes) is the boundary case for the
    // chunked copy loop: the full-chunk loop must consume all 32 bytes, leaving zero for the
    // partial-chunk branch. A clean finish proves the loop bound handles the exact multiple.
    U8 msg[32];
    for (FwSizeType i = 0; i < sizeof msg; i++) {
        msg[i] = static_cast<U8>(i + 1);
    }
    this->enqueueSerialIn(2, msg, sizeof msg);

    StagedAsset file_asset(*this, "serial_recv_chunk32.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 303, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

TEST_F(WasmSequencerTester, SerialRecvNonBlockingEmptyReturnsEmptyStatus) {
    // A NONBLOCKING serial_recv on an empty queue does not park: the host resumes the guest
    // immediately with status 1 (EMPTY) and writes nothing into the data buffer. The guest
    // asserts both, so a clean finish proves the empty-nonblocking fast path.
    StagedAsset file_asset(*this, "serial_recv_nonblock_empty.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 304, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
}

TEST_F(WasmSequencerTester, SerialRecvInvalidPortTraps) {
    // serial_recv with an out-of-range port index is rejected in the host function with a
    // trap before parking to wait.
    StagedAsset file_asset(*this, "serial_recv_badport.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 305, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPort_SIZE(1);
    // Requested index 5 is out of range; numPorts == NUM_SERIALIN_INPUT_PORTS == 5.
    ASSERT_EVENTS_HostFunctionInvalidPort(0, WasmSequencer_HostFunction::SERIAL_RECV, 5, 5);
    this->assertSequenceFailureCount(1);
}

TEST_F(WasmSequencerTester, SerialRecvInvalidBlockingTypeTraps) {
    // serial_recv with an out-of-range block_type (256) must trap. 256's low byte is 0, so a
    // truncated-U8 validation would wrongly accept it as BLOCKING; the host must validate the
    // full-width value and emit InvalidBlockingTypeValue instead.
    StagedAsset file_asset(*this, "serial_recv_badblock.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 306, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_InvalidBlockingTypeValue_SIZE(1);
    ASSERT_EVENTS_InvalidBlockingTypeValue(0, 256);
    this->assertSequenceFailureCount(1);
}

TEST_F(WasmSequencerTester, SerialRecvMessageLargerThanGuestBufferFails) {
    // The guest declares an 8-byte buffer (data_size = 4 in the module) but the injected
    // message is 8 bytes -- larger than the guest buffer. The host must fail the host call
    // (BufferTooSmall) rather than overflow guest memory, and leave the message on the queue.
    const U8 msg[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
    this->enqueueSerialIn(2, msg, sizeof msg);

    StagedAsset file_asset(*this, "serial_recv_toobig.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 307, file, BLOCK, {});
    // The sequence starts and ends in IDLE (it fails on recv), so dispatchUntilControllerState
    // would be a no-op; drain the queue explicitly instead.
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_BufferTooSmall_SIZE(1);
    // Guest buffer capacity (4) is smaller than the queued message (8).
    ASSERT_EVENTS_BufferTooSmall(0, WasmSequencer_HostFunction::SERIAL_RECV, 4, 8);
    this->assertSequenceFailureCount(1);
}

TEST_F(WasmSequencerTester, SerialRecvBadPointerFails) {
    // A blocking serial_recv with an out-of-bounds actual_size_ptr: when the host dequeues a
    // delivered message, the first spacewasm_mem_write (of the size to actual_size_ptr) fails
    // -> HostFunctionInvalidPointer(SERIAL_RECV, ERR_MEM_OUT_OF_BOUNDS) -> stmtFailure -> IDLE.
    // This also proves the failure event reports the real spacewasm status, not a stale OK.
    const U8 msg[4] = {0x11, 0x22, 0x33, 0x44};
    this->enqueueSerialIn(2, msg, sizeof msg);

    StagedAsset file_asset(*this, "serial_recv_badptr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 308, file, BLOCK, {});
    // The sequence starts and ends in IDLE (it fails on recv), so dispatchUntilControllerState
    // would be a no-op; drain the queue explicitly instead.
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::SERIAL_RECV,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
}

TEST_F(WasmSequencerTester, SerialRecvBadDataPointerPartialChunkFails) {
    // actual_size_ptr is valid, so the size write succeeds, but data_ptr is out of bounds.
    // With a small (4-byte) message the payload copy runs through the final partial-chunk
    // branch, whose spacewasm_mem_write fails -> HostFunctionInvalidPointer(SERIAL_RECV) ->
    // stmtFailure -> IDLE. Covers the partial-chunk write-failure path (distinct from the
    // actual_size_ptr write failure above).
    const U8 msg[4] = {0x11, 0x22, 0x33, 0x44};
    this->enqueueSerialIn(2, msg, sizeof msg);

    StagedAsset file_asset(*this, "serial_recv_baddata.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 309, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::SERIAL_RECV,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
}

TEST_F(WasmSequencerTester, SerialRecvBadDataPointerFullChunkFails) {
    // Same as above but with a 40-byte message so the copy takes the multi-chunk path: the
    // FIRST full 32-byte chunk write to the OOB data_ptr fails, covering the full-chunk
    // write-failure branch (distinct from the final partial-chunk branch).
    U8 msg[40];
    for (FwSizeType i = 0; i < sizeof msg; i++) {
        msg[i] = static_cast<U8>(i + 1);
    }
    this->enqueueSerialIn(2, msg, sizeof msg);

    StagedAsset file_asset(*this, "serial_recv_baddata_big.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 310, file, BLOCK, {});
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_HostFunctionInvalidPointer_SIZE(1);
    ASSERT_EVENTS_HostFunctionInvalidPointer(0, WasmSequencer_HostFunction::SERIAL_RECV,
                                             WasmSequencer_Status::ERR_MEM_OUT_OF_BOUNDS);
    this->assertSequenceFailureCount(1);
}

// ----------------------------------------------------------------------
// serialIn inbound queue: enqueue framing and queue-full behavior (white-box)
// ----------------------------------------------------------------------

TEST_F(WasmSequencerTester, SerialInEnqueueFramesRawSizePrefixedMessages) {
    // serialIn_handler must enqueue each message as a raw [U32 size][payload] frame using the
    // raw CircularBuffer::serialize overload (no length token). Enqueue two messages and
    // verify the queue holds exactly (4 + size) bytes per frame and the raw big-endian size
    // prefix of the first frame reads back correctly -- catching a wrong serialize overload.
    const U8 msgA[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
    const U8 msgB[3] = {0xAA, 0xBB, 0xCC};

    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE> a(msgA, sizeof msgA);
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE> b(msgB, sizeof msgB);
    this->invoke_to_serialIn(0, a);
    this->invoke_to_serialIn(0, b);
    this->dispatchAll();

    Types::CircularBuffer& queue = this->serialInQueue(0);
    const FwSizeType expected = (sizeof(U32) + sizeof msgA) + (sizeof(U32) + sizeof msgB);
    ASSERT_EQ(queue.get_allocated_size(), expected);

    // The first frame's size prefix (big-endian U32, matching peek(U32)) reads back as msgA's
    // length, and the first payload byte follows immediately after the 4-byte prefix.
    U32 firstSize = 0;
    ASSERT_EQ(queue.peek(firstSize), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(firstSize, static_cast<U32>(sizeof msgA));
    U8 firstPayloadByte = 0;
    ASSERT_EQ(queue.peek(firstPayloadByte, sizeof(U32)), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(firstPayloadByte, msgA[0]);
}

TEST_F(WasmSequencerTester, SerialInQueueFullDropsOldest) {
    // The default queue-full behavior is DROP_OLDEST. Fill the queue near capacity, then push
    // a message that does not fit; the handler drops whole frames from the front until the new
    // message fits. Verify the newest message survives and the total never exceeds capacity.
    ASSERT_EQ(Svc::WasmSequencerConfig::SERIAL_IN_QUEUE_FULL_BEHAVIOR,
              Svc::WasmSequencerConfig::SerialInQueueFullBehavior::DROP_OLDEST);

    Types::CircularBuffer& queue = this->serialInQueue(0);
    const FwSizeType capacity = queue.get_capacity();

    // Each frame is (4 + payload) bytes. Use ~1/3-capacity payloads so three frames nearly
    // fill the queue and a fourth forces at least one drop.
    const FwSizeType payload = (capacity / 3) - sizeof(U32);
    U8 buf[Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE];

    for (U32 n = 0; n < 3; n++) {
        for (FwSizeType i = 0; i < payload; i++) {
            buf[i] = static_cast<U8>(n);  // frame n filled with byte value n
        }
        Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE> m(buf, payload);
        this->invoke_to_serialIn(0, m);
        this->dispatchAll();
    }
    const FwSizeType beforeFree = queue.get_free_size();

    // Push a fourth frame (byte value 3) that cannot fit alongside all three -> DROP_OLDEST.
    for (FwSizeType i = 0; i < payload; i++) {
        buf[i] = static_cast<U8>(3);
    }
    Fw::LinearBufferTemplate<Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE> m(buf, payload);
    this->invoke_to_serialIn(0, m);
    this->dispatchAll();

    // The frame did not fit before the drop, but the queue still holds it now and stays within
    // capacity -- the oldest frame(s) were dropped to make room.
    ASSERT_LT(beforeFree, sizeof(U32) + payload);
    ASSERT_LE(queue.get_allocated_size(), capacity);

    // The oldest surviving frame is no longer frame 0: its size prefix is intact and its first
    // payload byte identifies which frame now sits at the front (frame 1 or later, never 0).
    U32 frontSize = 0;
    ASSERT_EQ(queue.peek(frontSize), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(frontSize, static_cast<U32>(payload));
    U8 frontByte = 0;
    ASSERT_EQ(queue.peek(frontByte, sizeof(U32)), Fw::FW_SERIALIZE_OK);
    ASSERT_GT(frontByte, static_cast<U8>(0)) << "oldest frame (byte 0) should have been dropped";
}

TEST_F(WasmSequencerTester, SerialInOversizedFrameDroppedNotAsserted) {
    // serialIn is fed by external hardware/ground data, so a frame larger than the
    // queue could EVER hold (payload > capacity minus the 4-byte length header) must be
    // dropped with a warning rather than asserting and crashing flight software.
    Types::CircularBuffer& queue = this->serialInQueue(0);
    const FwSizeType capacity = queue.get_capacity();
    const FwSizeType headerSize = sizeof(U32);
    const FwSizeType maxPayload = capacity - headerSize;

    // One byte too large to ever fit, so this is a reject (not a DROP_OLDEST eviction).
    const FwSizeType oversized = maxPayload + 1;
    ASSERT_LE(oversized, static_cast<FwSizeType>(Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE));
    U8 payload[Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE];
    for (FwSizeType i = 0; i < oversized; i++) {
        payload[i] = static_cast<U8>(i);
    }

    this->enqueueSerialIn(0, payload, oversized);

    // Nothing was enqueued and the component did not assert; the oversized frame was
    // reported and dropped.
    ASSERT_EQ(queue.get_allocated_size(), static_cast<FwSizeType>(0));
    ASSERT_EVENTS_SerialInFrameTooLarge_SIZE(1);
    ASSERT_EVENTS_SerialInFrameTooLarge(0, 0, static_cast<U32>(oversized), static_cast<U32>(maxPayload));

    // The queue is still usable: a normally-sized frame enqueues afterward and no
    // further drop is reported.
    const U8 ok[4] = {0x01, 0x02, 0x03, 0x04};
    this->enqueueSerialIn(0, ok, sizeof ok);
    ASSERT_EQ(queue.get_allocated_size(), static_cast<FwSizeType>(sizeof(U32) + sizeof ok));
    ASSERT_EVENTS_SerialInFrameTooLarge_SIZE(1);
}

TEST_F(WasmSequencerTester, SerialInMaxSizeFrameEnqueued) {
    // The exact-fit boundary: a payload of (capacity - header) is the largest frame
    // that fits and MUST be enqueued, not dropped. Guards the '>' drop guard against
    // regressing to '>=' (which would reject the maximum legal frame).
    Types::CircularBuffer& queue = this->serialInQueue(0);
    const FwSizeType capacity = queue.get_capacity();
    const FwSizeType headerSize = sizeof(U32);
    const FwSizeType maxPayload = capacity - headerSize;

    U8 payload[Svc::WasmSequencerConfig::MAX_SERIAL_OUT_SIZE];
    for (FwSizeType i = 0; i < maxPayload; i++) {
        payload[i] = static_cast<U8>(i);
    }

    this->enqueueSerialIn(0, payload, maxPayload);

    // Enqueued as a single [U32 size][payload] frame that fills the queue exactly, with
    // no oversized-frame warning.
    ASSERT_EVENTS_SerialInFrameTooLarge_SIZE(0);
    ASSERT_EQ(queue.get_allocated_size(), capacity);
    U32 framedSize = 0;
    ASSERT_EQ(queue.peek(framedSize), Fw::FW_SERIALIZE_OK);
    ASSERT_EQ(framedSize, static_cast<U32>(maxPayload));
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

    StagedAsset empty_asset(*this, "empty.wasm");
    const Fw::String& empty = empty_asset.file();
    StagedAsset unreachable_asset(*this, "unreachable.wasm");
    const Fw::String& unreachable = unreachable_asset.file();
    StagedAsset malformed_asset(*this, "malformed.wasm");
    const Fw::String& malformed = malformed_asset.file();
    StagedAsset loop_asset(*this, "loop.wasm");
    const Fw::String& loop = loop_asset.file();
    StagedAsset exit0_asset(*this, "exit0.wasm");
    const Fw::String& exit0 = exit0_asset.file();
    StagedAsset panic_asset(*this, "panic.wasm");
    const Fw::String& panic = panic_asset.file();

    // Sequence-success/cancel events map 1:1 with those modes, but the "failed"
    // telemetry counter aggregates two kinds of failure: an execution-failure event
    // (SequenceExited / SequencePanic / SequenceTrapped / SequenceHostFailure) and
    // ModuleLoadFailed (a load failure). Track the execution-failure event count
    // separately from the cumulative counter.
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
    this->assertSequenceFailureCount(seqFailedEvents);

    // 3. LOAD empty from IDLE -> READY, but no run: counters unchanged.
    this->sendCmd_LOAD(0, 3, empty, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);

    // 4. RUN malformed from READY -> load fails (resetStore then load). A failed
    //    load invalidates the store and returns to IDLE. It reports ModuleLoadFailed
    //    (not an execution-failure event) but still counts toward SequencesFailed.
    this->sendCmd_RUN(0, 4, malformed, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::IDLE);
    failedCount++;
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_EVENTS_ModuleLoadFailed_SIZE(1);
    this->assertSequenceFailureCount(seqFailedEvents);  // unchanged: a load failure is not an execution-failure event

    // 5. RUN loop NO_BLOCK, then CANCEL mid-spin -> cancelled, back to IDLE.
    this->sendCmd_RUN(0, 5, loop, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);
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
    this->assertSequenceFailureCount(seqFailedEvents);

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
}

// spacewasm_panic is the #[panic_handler] the whole fprime_spacewasm staticlib
// registers (see spacewasm_c_api's "provide-panic-handler" feature): it fires on
// any internal Rust panic in the engine, not on a guest trap (that's the separate
// fprime_v1.panic host import exercised by panic.wasm above). It is declared
// `-> !` -- it must not return -- so its only contract to verify is that it
// reports the panic and maps it to an FSW assertion.
//
// A death test (fork + expect abort) would exercise this too, but the forked
// child's coverage counters are lost on abort() before gcov's atexit flush
// runs -- the lines would execute but never show as covered. Swap in the
// non-aborting ::Test::UnitTestAssert hook instead so the assert fires in this
// process and gcov sees it.
TEST_F(WasmSequencerTester, SpacewasmPanicBridgeAssertsFsw) {
    ::Test::UnitTestAssert assertHook;
    const char filename[] = "engine.rs";
    const char msg[] = "internal invariant violated";
    spacewasm_panic(reinterpret_cast<const U8*>(filename), sizeof(filename) - 1, 42, reinterpret_cast<const U8*>(msg),
                    sizeof(msg) - 1);
    ASSERT_TRUE(assertHook.assertFailed());

    ::Test::UnitTestAssert::File file;
    FwSizeType lineNo = 0, numArgs = 0;
    FwAssertArgType arg1 = 0, arg2 = 0, arg3 = 0, arg4 = 0, arg5 = 0, arg6 = 0;
    assertHook.retrieveAssert(file, lineNo, numArgs, arg1, arg2, arg3, arg4, arg5, arg6);
    ASSERT_NE(strstr(file.toChar(), "WasmSequencerHelpers.cpp"), nullptr);
}

// ----------------------------------------------------------------------
// GLOBAL_SET_* / GLOBAL_GET
//
// The global commands operate directly on the loaded store (they are plain
// async command handlers, not routed through the controller state machine).
// `globals.wasm` exports one mutable global of each Wasm 1.0 value type
// (g_i32=100, g_i64=1000, g_f32=1.5, g_f64=2.5) plus an immutable const
// (c_i32=7). The float initial/set values are all exactly representable so the
// generated ASSERT_EVENTS_GlobalValue{F32,F64} exact-equality checks hold.
//
// Status codes come straight from spacewasm_c_api::spacewasm_set_global /
// _get_global / _find_global / _find_module:
//   * unknown module / global / out-of-range index -> ERR_NOT_FOUND (7)
//   * set with a value tag != the global's type     -> ERR_GLOBAL_TYPE_MISMATCH (164)
//   * set an immutable (const) global (type matches) -> ERR_GLOBAL_IS_NOT_MUTABLE (150)
// Note set_global checks the type BEFORE mutability, so a const global written
// with a mismatched type surfaces the type-mismatch error, not the mutability one.
// ----------------------------------------------------------------------

// --- GLOBAL_GET: read the declared initial value of each typed global -------

TEST_F(WasmSequencerTester, GlobalGetReadsInitialI32) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 60, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    // Unnamed LOAD -> module name is "" (see LOAD_cmdHandler). GET reads the
    // declared init value (100) and emits the typed value event.
    this->sendCmd_GLOBAL_GET(0, 61, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();

    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_GlobalValueI32_SIZE(1);
    ASSERT_EVENTS_GlobalValueI32(0, "", "g_i32", 100);
    ASSERT_EVENTS_GlobalGetFailed_SIZE(0);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_GET, 61, Fw::CmdResponse::OK);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, GlobalGetReadsInitialI64) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 62, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_GET(0, 63, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i64"));
    this->dispatchAll();

    ASSERT_EVENTS_GlobalValueI64_SIZE(1);
    ASSERT_EVENTS_GlobalValueI64(0, "", "g_i64", static_cast<I64>(1000));
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_GET, 63, Fw::CmdResponse::OK);
}

TEST_F(WasmSequencerTester, GlobalGetReadsInitialF32) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 64, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_GET(0, 65, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f32"));
    this->dispatchAll();

    ASSERT_EVENTS_GlobalValueF32_SIZE(1);
    ASSERT_EVENTS_GlobalValueF32(0, "", "g_f32", 1.5f);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_GET, 65, Fw::CmdResponse::OK);
}

TEST_F(WasmSequencerTester, GlobalGetReadsInitialF64) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 66, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_GET(0, 67, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f64"));
    this->dispatchAll();

    ASSERT_EVENTS_GlobalValueF64_SIZE(1);
    ASSERT_EVENTS_GlobalValueF64(0, "", "g_f64", 2.5);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_GET, 67, Fw::CmdResponse::OK);
}

TEST_F(WasmSequencerTester, GlobalGetReadsConstGlobal) {
    // A const (immutable) global is still readable via GET.
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 68, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_GET(0, 69, Fw::CmdStringArg(""), Fw::CmdStringArg("c_i32"));
    this->dispatchAll();

    ASSERT_EVENTS_GlobalValueI32_SIZE(1);
    ASSERT_EVENTS_GlobalValueI32(0, "", "c_i32", 7);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_GET, 69, Fw::CmdResponse::OK);
}

// --- GLOBAL_SET_* then GLOBAL_GET: full round trip per type -----------------

TEST_F(WasmSequencerTester, GlobalSetGetRoundTripI32) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 70, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_I32(0, 71, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"), -12345);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_I32, 71, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GlobalSetFailed_SIZE(0);

    // GET observes the value the SET just wrote.
    this->sendCmd_GLOBAL_GET(0, 72, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI32_SIZE(1);
    ASSERT_EVENTS_GlobalValueI32(0, "", "g_i32", -12345);
    ASSERT_CMD_RESPONSE(2, OPCODE_GLOBAL_GET, 72, Fw::CmdResponse::OK);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
}

TEST_F(WasmSequencerTester, GlobalSetGetRoundTripI64) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 73, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    // A value that does not fit in 32 bits, to prove the full i64 round-trips.
    const I64 big = static_cast<I64>(0x0123456789ABCDEFLL);
    this->sendCmd_GLOBAL_SET_I64(0, 74, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i64"), big);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_I64, 74, Fw::CmdResponse::OK);

    this->sendCmd_GLOBAL_GET(0, 75, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i64"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI64_SIZE(1);
    ASSERT_EVENTS_GlobalValueI64(0, "", "g_i64", big);
    ASSERT_CMD_RESPONSE(2, OPCODE_GLOBAL_GET, 75, Fw::CmdResponse::OK);
}

TEST_F(WasmSequencerTester, GlobalSetGetRoundTripF32) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 76, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_F32(0, 77, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f32"), 3.25f);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_F32, 77, Fw::CmdResponse::OK);

    this->sendCmd_GLOBAL_GET(0, 78, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueF32_SIZE(1);
    ASSERT_EVENTS_GlobalValueF32(0, "", "g_f32", 3.25f);
    ASSERT_CMD_RESPONSE(2, OPCODE_GLOBAL_GET, 78, Fw::CmdResponse::OK);
}

TEST_F(WasmSequencerTester, GlobalSetGetRoundTripF64) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 79, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_F64(0, 80, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f64"), 6.5);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_F64, 80, Fw::CmdResponse::OK);

    this->sendCmd_GLOBAL_GET(0, 81, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f64"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueF64_SIZE(1);
    ASSERT_EVENTS_GlobalValueF64(0, "", "g_f64", 6.5);
    ASSERT_CMD_RESPONSE(2, OPCODE_GLOBAL_GET, 81, Fw::CmdResponse::OK);
}

// --- Named-module addressing ------------------------------------------------

TEST_F(WasmSequencerTester, GlobalSetGetNamedModule) {
    // LOAD gives the module an explicit name; the global commands must
    // resolve globals by that same name rather than the empty string.
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 82, file, Fw::CmdStringArg("mod"));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_I32(0, 83, Fw::CmdStringArg("mod"), Fw::CmdStringArg("g_i32"), 555);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_I32, 83, Fw::CmdResponse::OK);

    this->sendCmd_GLOBAL_GET(0, 84, Fw::CmdStringArg("mod"), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI32_SIZE(1);
    ASSERT_EVENTS_GlobalValueI32(0, "mod", "g_i32", 555);
    ASSERT_CMD_RESPONSE(2, OPCODE_GLOBAL_GET, 84, Fw::CmdResponse::OK);

    // The empty-string name no longer resolves (module was loaded as "mod").
    this->sendCmd_GLOBAL_GET(0, 85, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalGetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalGetFailed(0, "", "g_i32", WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_CMD_RESPONSE(3, OPCODE_GLOBAL_GET, 85, Fw::CmdResponse::EXECUTION_ERROR);
}

// --- Failure: type mismatch (set the wrong-typed value) ---------------------

TEST_F(WasmSequencerTester, GlobalSetTypeMismatchI64OnI32) {
    // g_i32 is an i32 global; setting it via the I64 command is a type mismatch.
    // set_global checks type before mutability, so the code is TYPE_MISMATCH.
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 86, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_I64(0, 87, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"), static_cast<I64>(1));
    this->dispatchAll();

    ASSERT_EVENTS_GlobalSetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalSetFailed(0, "", "g_i32", WasmSequencer_Status::ERR_GLOBAL_TYPE_MISMATCH);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_I64, 87, Fw::CmdResponse::EXECUTION_ERROR);
    // The failed set left the global untouched: it still reads its init value.
    this->sendCmd_GLOBAL_GET(0, 88, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI32(0, "", "g_i32", 100);
}

TEST_F(WasmSequencerTester, GlobalSetTypeMismatchF32OnI32) {
    // Setting the i32 global via the F32 command is likewise a type mismatch.
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 89, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_F32(0, 90, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"), 1.0f);
    this->dispatchAll();

    ASSERT_EVENTS_GlobalSetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalSetFailed(0, "", "g_i32", WasmSequencer_Status::ERR_GLOBAL_TYPE_MISMATCH);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_F32, 90, Fw::CmdResponse::EXECUTION_ERROR);
}

// --- Failure: immutable global ----------------------------------------------

TEST_F(WasmSequencerTester, GlobalSetConstIsNotMutable) {
    // c_i32 is a const i32. Setting it with a matching-typed value passes the
    // type check and then fails the mutability check.
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 91, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_I32(0, 92, Fw::CmdStringArg(""), Fw::CmdStringArg("c_i32"), 999);
    this->dispatchAll();

    ASSERT_EVENTS_GlobalSetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalSetFailed(0, "", "c_i32", WasmSequencer_Status::ERR_GLOBAL_IS_NOT_MUTABLE);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_I32, 92, Fw::CmdResponse::EXECUTION_ERROR);
    // The const global is unchanged.
    this->sendCmd_GLOBAL_GET(0, 93, Fw::CmdStringArg(""), Fw::CmdStringArg("c_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI32(0, "", "c_i32", 7);
}

// --- Failure: unknown global / unknown module -------------------------------

TEST_F(WasmSequencerTester, GlobalSetUnknownGlobalNotFound) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 94, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_I32(0, 95, Fw::CmdStringArg(""), Fw::CmdStringArg("nope"), 1);
    this->dispatchAll();

    ASSERT_EVENTS_GlobalSetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalSetFailed(0, "", "nope", WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_I32, 95, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
}

TEST_F(WasmSequencerTester, GlobalGetUnknownGlobalNotFound) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 96, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_GET(0, 97, Fw::CmdStringArg(""), Fw::CmdStringArg("nope"));
    this->dispatchAll();

    ASSERT_EVENTS_GlobalGetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalGetFailed(0, "", "nope", WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_EVENTS_GlobalValueI32_SIZE(0);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_GET, 97, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(WasmSequencerTester, GlobalSetUnknownModuleNotFound) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 98, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    // Module loaded as "" (unnamed); a different name does not resolve.
    this->sendCmd_GLOBAL_SET_I32(0, 99, Fw::CmdStringArg("other"), Fw::CmdStringArg("g_i32"), 1);
    this->dispatchAll();

    ASSERT_EVENTS_GlobalSetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalSetFailed(0, "other", "g_i32", WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_I32, 99, Fw::CmdResponse::EXECUTION_ERROR);
}

TEST_F(WasmSequencerTester, GlobalGetUnknownModuleNotFound) {
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 100, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_GET(0, 101, Fw::CmdStringArg("other"), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();

    ASSERT_EVENTS_GlobalGetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalGetFailed(0, "other", "g_i32", WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_GET, 101, Fw::CmdResponse::EXECUTION_ERROR);
}

// --- From IDLE (empty store, no modules loaded) -----------------------------

TEST_F(WasmSequencerTester, GlobalSetFromIdleModuleNotFound) {
    // From IDLE the store exists (IDLE's entry action creates an empty store)
    // but has no modules, so the lookup fails cleanly with NOT_FOUND rather than
    // crashing. State is unaffected.
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);

    this->sendCmd_GLOBAL_SET_I32(0, 102, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"), 1);
    this->dispatchAll();

    ASSERT_EVENTS_GlobalSetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalSetFailed(0, "", "g_i32", WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_CMD_RESPONSE(0, OPCODE_GLOBAL_SET_I32, 102, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
    ASSERT_FROM_PORT_HISTORY_SIZE(0);
}

TEST_F(WasmSequencerTester, GlobalGetFromIdleModuleNotFound) {
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);

    this->sendCmd_GLOBAL_GET(0, 103, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();

    ASSERT_EVENTS_GlobalGetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalGetFailed(0, "", "g_i32", WasmSequencer_Status::ERR_NOT_FOUND);
    ASSERT_EVENTS_GlobalValueI32_SIZE(0);
    ASSERT_CMD_RESPONSE(0, OPCODE_GLOBAL_GET, 103, Fw::CmdResponse::EXECUTION_ERROR);
    ASSERT_EQ(this->controllerState(), ControllerState::IDLE);
}

// --- Interaction with execution ---------------------------------------------

TEST_F(WasmSequencerTester, GlobalSetPersistsIntoInvoke) {
    // global_incr.wasm's main does g_i32 += 10. LOAD keeps the store, so a
    // GLOBAL_SET before INVOKE is observed by main, and a GLOBAL_GET after sees
    // main's write-back. This proves the set mutates the same store the engine
    // runs against (not a throwaway copy).
    StagedAsset file_asset(*this, "global_incr.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 104, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    // Seed the global to 100.
    this->sendCmd_GLOBAL_SET_I32(0, 105, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"), 100);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_I32, 105, Fw::CmdResponse::OK);

    // INVOKE main (BLOCK) -> reads 100, writes 110. Store is retained after INVOKE.
    this->sendCmd_INVOKE(0, 106, Fw::CmdStringArg(""), BLOCK, {});
    this->dispatchAll();
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);
    ASSERT_CMD_RESPONSE(2, OPCODE_INVOKE, 106, Fw::CmdResponse::OK);

    // GET now reflects main's mutation.
    this->sendCmd_GLOBAL_GET(0, 107, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI32_SIZE(1);
    ASSERT_EVENTS_GlobalValueI32(0, "", "g_i32", 110);
    ASSERT_CMD_RESPONSE(3, OPCODE_GLOBAL_GET, 107, Fw::CmdResponse::OK);
}

TEST_F(WasmSequencerTester, GlobalSetOverwrittenAfterRunResetsStore) {
    // RUN (unlike INVOKE) does resetStore before load, so any pre-RUN global
    // state is discarded and the module comes back at its declared init. Here
    // main runs once as part of RUN (g_i32: 0 -> 10); a GET afterward reads 10,
    // and a fresh RUN resets to 0 then runs main again (-> 10), NOT 20.
    StagedAsset file_asset(*this, "global_incr.wasm");
    const Fw::String& file = file_asset.file();

    this->sendCmd_RUN(0, 108, file, BLOCK, {});
    this->dispatchUntilControllerState(ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(1);

    this->sendCmd_GLOBAL_GET(0, 109, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI32(0, "", "g_i32", 10);

    // Second RUN resets the store: main sees the init 0 again, not the prior 10.
    // The component is already in READY and the RUN returns to READY, so drain the
    // queue rather than waiting on a state change that has effectively already
    // happened (cf. LoadStartModuleTwiceDoesNotWedge).
    this->sendCmd_RUN(0, 110, file, BLOCK, {});
    this->dispatchAll();
    ASSERT_EQ(this->controllerState(), ControllerState::READY);
    ASSERT_EVENTS_SequenceSucceeded_SIZE(2);

    this->sendCmd_GLOBAL_GET(0, 111, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI32_SIZE(2);
    ASSERT_EVENTS_GlobalValueI32(1, "", "g_i32", 10);
}

TEST_F(WasmSequencerTester, GlobalSetAllTypesIndependent) {
    // Setting each typed global does not disturb the others: set all four, then
    // read all four back and confirm each holds exactly what was written.
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 112, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_I32(0, 113, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"), 11);
    this->sendCmd_GLOBAL_SET_I64(0, 114, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i64"), static_cast<I64>(22));
    this->sendCmd_GLOBAL_SET_F32(0, 115, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f32"), 0.5f);
    this->sendCmd_GLOBAL_SET_F64(0, 116, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f64"), 0.25);
    this->dispatchAll();
    ASSERT_EVENTS_GlobalSetFailed_SIZE(0);

    this->sendCmd_GLOBAL_GET(0, 117, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->sendCmd_GLOBAL_GET(0, 118, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i64"));
    this->sendCmd_GLOBAL_GET(0, 119, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f32"));
    this->sendCmd_GLOBAL_GET(0, 120, Fw::CmdStringArg(""), Fw::CmdStringArg("g_f64"));
    this->dispatchAll();

    ASSERT_EVENTS_GlobalValueI32_SIZE(1);
    ASSERT_EVENTS_GlobalValueI32(0, "", "g_i32", 11);
    ASSERT_EVENTS_GlobalValueI64_SIZE(1);
    ASSERT_EVENTS_GlobalValueI64(0, "", "g_i64", static_cast<I64>(22));
    ASSERT_EVENTS_GlobalValueF32_SIZE(1);
    ASSERT_EVENTS_GlobalValueF32(0, "", "g_f32", 0.5f);
    ASSERT_EVENTS_GlobalValueF64_SIZE(1);
    ASSERT_EVENTS_GlobalValueF64(0, "", "g_f64", 0.25);
}

// --- F64 set failure (symmetry with the I32/I64/F32 failure-event fix) -------

TEST_F(WasmSequencerTester, GlobalSetF64TypeMismatchEmitsFailure) {
    // Rounds out the SET-failure coverage: the F64 command must also emit
    // GlobalSetFailed on error, matching the I32/I64/F32 handlers. g_i32 is an
    // i32 global, so the F64 command's SPACEWASM_F64 tag is a type mismatch.
    StagedAsset file_asset(*this, "globals.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_LOAD(0, 121, file, Fw::CmdStringArg(""));
    this->dispatchUntilControllerState(ControllerState::READY);

    this->sendCmd_GLOBAL_SET_F64(0, 122, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"), 1.0);
    this->dispatchAll();

    ASSERT_EVENTS_GlobalSetFailed_SIZE(1);
    ASSERT_EVENTS_GlobalSetFailed(0, "", "g_i32", WasmSequencer_Status::ERR_GLOBAL_TYPE_MISMATCH);
    ASSERT_CMD_RESPONSE(1, OPCODE_GLOBAL_SET_F64, 122, Fw::CmdResponse::EXECUTION_ERROR);
}

// --- Dispatched mid-sequence (engine RUNNING) -------------------------------

TEST_F(WasmSequencerTester, GlobalGetSetWhileSequencePaused) {
    // The GLOBAL_* commands are plain async handlers that touch the live store
    // directly; unlike RUN/LOAD/INVOKE they are NOT gated by the controller and
    // are never rejected with BUSY while a sequence is in flight. Drive
    // global_loop.wasm (a long busy-loop that also exports g_i32) into
    // RUNNING_PAUSED with tiny fuel, then GET and SET the global mid-run.
    this->paramSet_INSTRUCTION_FUEL(static_cast<FwSizeType>(10), Fw::ParamValid::VALID);

    StagedAsset file_asset(*this, "global_loop.wasm");
    const Fw::String& file = file_asset.file();
    this->sendCmd_RUN(0, 123, file, NO_BLOCK, {});
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_SPINNING);

    // Pause so the engine parks quiescently (no self-posted spin messages),
    // making the global command the only work dispatchAll() advances.
    this->sendCmd_PAUSE(0, 124);
    this->dispatchUntilInterpreterState(InterpreterState::RUNNING_PAUSED);
    ASSERT_EQ(this->controllerState(), ControllerState::RUNNING_MAIN);

    // GET reads the live global (its declared init 42) without disturbing the run.
    // The RUN (NO_BLOCK -> OK at load) and PAUSE responses already occupy cmd
    // response history indices 0 and 1, so the global responses start at index 2.
    this->sendCmd_GLOBAL_GET(0, 125, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI32_SIZE(1);
    ASSERT_EVENTS_GlobalValueI32(0, "", "g_i32", 42);
    ASSERT_CMD_RESPONSE(2, OPCODE_GLOBAL_GET, 125, Fw::CmdResponse::OK);
    // Still paused mid-sequence: the global command did not advance or end the run.
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_PAUSED);
    ASSERT_EQ(this->controllerState(), ControllerState::RUNNING_MAIN);

    // SET is likewise accepted mid-run and mutates the live store.
    this->sendCmd_GLOBAL_SET_I32(0, 126, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"), 7);
    this->dispatchAll();
    ASSERT_CMD_RESPONSE(3, OPCODE_GLOBAL_SET_I32, 126, Fw::CmdResponse::OK);
    ASSERT_EVENTS_GlobalSetFailed_SIZE(0);
    ASSERT_EQ(this->interpreterState(), InterpreterState::RUNNING_PAUSED);

    this->sendCmd_GLOBAL_GET(0, 127, Fw::CmdStringArg(""), Fw::CmdStringArg("g_i32"));
    this->dispatchAll();
    ASSERT_EVENTS_GlobalValueI32_SIZE(2);
    ASSERT_EVENTS_GlobalValueI32(1, "", "g_i32", 7);

    // Let the sequence finish so the component tears down cleanly.
    this->sendCmd_CANCEL(0, 128);
    this->dispatchUntilControllerState(ControllerState::IDLE);
}

}  // namespace Svc

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
