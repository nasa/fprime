// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

#include "Fw/Cmd/CmdResponseEnumAc.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleIdxAliasAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_SequencerStateMachine_StateEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_TrapReasonEnumAc.hpp"
#include "config/FwAssertArgTypeAliasAc.h"
#include "spacewasm.h"

namespace Svc {

namespace {
// FIXME(tumbar) This only allows a singleton WasmSequencer: spacewasm_set_global_allocator
//               installs a single process-wide backend, so only one instance may
//               register its pool at a time.
WasmSequencer* g_activeSequencer = nullptr;
}  // namespace

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

WasmSequencer ::WasmSequencer(const char* const compName)
    : WasmSequencerComponentBase(compName),
      m_page_used_mask(0),
      m_guest_offset(0),
      m_wasm(nullptr),
      m_moduleIndex(0),
      m_hasPendingLoadCmd(false),
      m_hasPendingLoad(false),
      m_pendingTimer(),
      m_hasPendingTimer(false),
      m_breakBeforeNextLine(false),
      m_loadFile(nullptr),
      m_lastTrap(SPACEWASM_TRAP_NONE) {
    FW_ASSERT(g_activeSequencer == nullptr);
    g_activeSequencer = this;

    // Install the process-wide global page allocator, backed by this instance's
    // page pool. The store/bytecode live entirely within m_memory_pool.
    const I32 status = spacewasm_set_global_allocator(&wasmSeqGlobalAlloc, &wasmSeqGlobalDealloc, this);
    FW_ASSERT(status == 0, status);
}

WasmSequencer ::~WasmSequencer() {
    this->destroyStore();
    g_activeSequencer = nullptr;
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void WasmSequencer ::checkTimers_handler(FwIndexType portNum, U32 context) {
    // Drive the sleep-wake / statement-timeout checks in the state machine.
    this->sequencer_sendSignal_checkTimersIn();
}

void WasmSequencer ::cmdResponseIn_handler(FwIndexType portNum,
                                           FwOpcodeType opCode,
                                           U32 cmdSeq,
                                           const Fw::CmdResponse& response) {
    if (this->sequencer_getState() != WasmSequencer_SequencerStateMachine_State::RUNNING_AWAITING_RESPONSE) {
        this->sequencer_sendSignal_stmtResponse_unexpected();
    } else if (this->m_pendingHostFunction.kind != PendingHostInvocation::COMMAND) {
        this->sequencer_sendSignal_stmtResponse_unexpected();
    } else {
        // Respond to the host command by pushing a value to the Wasm operand stack
        spacewasm_value_t return_val;
        return_val.tag = SPACEWASM_I32;
        return_val.u.i32_ = static_cast<I32>(response.e);

        const auto status = spacewasm_resume_value(this->m_wasm, return_val);
        FW_ASSERT(status == SPACEWASM_OK, status);

        // Increment the cmdSeq to track unique cmd dispatch
        this->m_cmdSeq += 1;

        this->sequencer_sendSignal_stmtResponse_success();
    }
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void WasmSequencer ::RUN_cmdHandler(FwOpcodeType opCode,
                                    U32 cmdSeq,
                                    const Fw::CmdStringArg& fileName,
                                    const Svc::BlockState& block) {
    // RUN is only valid from IDLE
    if (this->sequencer_getState() != WasmSequencer_SequencerStateMachine_State::IDLE) {
        this->log_WARNING_LO_InvalidCommand(this->sequencer_getState());
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    switch (block) {
        case BlockState::BLOCK: {
            // Queue up a response when the execution finishes
            auto status = this->m_pendingFinishCmds.enqueue(PendingCmd(opCode, cmdSeq));
            if (status != Fw::Success::SUCCESS) {
                this->log_WARNING_HI_TooManyBlockingCommands();
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }

            break;
        }
        case BlockState::NO_BLOCK:
            // Respond to command after it loads
            FW_ASSERT(!this->m_hasPendingLoadCmd);
            this->m_pendingLoadCmd = PendingCmd(opCode, cmdSeq);
            this->m_hasPendingLoadCmd = true;
            break;
        default:
            FW_ASSERT(false, static_cast<FwAssertArgType>(block));
    }

    this->sequencer_sendSignal_cmd_RUN(Svc::WasmSequencer_SequenceExecutionArgs(fileName, Svc::SeqArgs{0, 0}));
}

void WasmSequencer ::WAIT_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    auto status = this->m_pendingFinishCmds.enqueue(PendingCmd(opCode, cmdSeq));
    if (status != Fw::Success::SUCCESS) {
        this->log_WARNING_HI_TooManyBlockingCommands();
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

void WasmSequencer ::LOAD_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& fileName) {
    // LOAD uses an empty module name (single unnamed module).
    this->LOAD_NAME_cmdHandler(opCode, cmdSeq, fileName, Fw::CmdStringArg(""));
}

void WasmSequencer ::LOAD_NAME_cmdHandler(FwOpcodeType opCode,
                                          U32 cmdSeq,
                                          const Fw::CmdStringArg& fileName,
                                          const Fw::CmdStringArg& name) {
    // Loading is only valid from IDLE.
    if (this->sequencer_getState() != WasmSequencer_SequencerStateMachine_State::IDLE) {
        this->log_WARNING_LO_InvalidCommand(this->sequencer_getState());
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    FW_ASSERT(!this->m_hasPendingLoadCmd);
    this->m_pendingLoadCmd = PendingCmd(opCode, cmdSeq);
    this->m_hasPendingLoadCmd = true;
    this->sequencer_sendSignal_cmd_LOAD(Svc::WasmSequencer_ModuleLoad(fileName, name));
}

void WasmSequencer ::INVOKE_cmdHandler(FwOpcodeType opCode,
                                       U32 cmdSeq,
                                       const Fw::CmdStringArg& module,
                                       const Svc::BlockState& block) {
    // INVOKE is only valid from IDLE.
    if (this->sequencer_getState() != Svc_WasmSequencer_SequencerStateMachine::State::IDLE) {
        this->log_WARNING_LO_InvalidCommand(this->sequencer_getState());
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    // Resolve the module name to its index within the engine.
    FW_ASSERT(this->m_wasm != nullptr);
    U32 moduleIdx = 0;
    const spacewasm_status_t findStatus = spacewasm_find_module(this->m_wasm, module.toChar(), &moduleIdx);
    if (findStatus != SPACEWASM_OK) {
        this->log_WARNING_LO_InvalidCommand(this->sequencer_getState());
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    switch (block) {
        case BlockState::BLOCK: {
            auto status = this->m_pendingFinishCmds.enqueue(PendingCmd(opCode, cmdSeq));
            if (status != Fw::Success::SUCCESS) {
                this->log_WARNING_HI_TooManyBlockingCommands();
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
                return;
            }
            break;
        }
        case BlockState::NO_BLOCK:
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        default:
            FW_ASSERT(false, block);
    }

    this->sequencer_sendSignal_cmd_INVOKE(static_cast<Svc::WasmSequencer_ModuleIdx>(moduleIdx));
}

void WasmSequencer ::CANCEL_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // Cancel is a no-op in IDLE; otherwise it aborts back to IDLE.
    this->sequencer_sendSignal_cmd_CANCEL();
    // Cancel returns immediately and always succeeds.
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::PAUSE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->sequencer_sendSignal_cmd_PAUSE();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::TRACE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    // TODO(tumbar) Dump a stack trace as an event once the interpreter exposes one.
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CONTINUE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    switch (this->sequencer_getState()) {
        case WasmSequencer_SequencerStateMachine_State::RUNNING_AWAITING_RESPONSE:
        case WasmSequencer_SequencerStateMachine_State::RUNNING_SLEEPING:
        case WasmSequencer_SequencerStateMachine_State::RUNNING_SPINNING:
            // Already running
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        case WasmSequencer_SequencerStateMachine_State::RUNNING_PAUSED:
            this->sequencer_sendSignal_cmd_CONTINUE();
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        default:
            this->log_WARNING_LO_InvalidCommand(this->sequencer_getState());
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            break;
    }
}

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_signalEntered(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->sequencer_sendSignal_entered();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_resetStore(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    Fw::ParamValid valid;
    this->createStore();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_invokeMainOfLastModule(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // Invoke main on the loaded module index
    this->Svc_WasmSequencer_SequencerStateMachine_action_invokeMain(smId, signal,
                                                                    Svc::WasmSequencer_ModuleIdx(this->m_moduleIndex));
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_invokeMain(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleIdx& value) {
    FW_ASSERT(this->m_wasm != nullptr);

    // Clear any pending main
    this->m_pendingRun = false;

    // Resolve the exported function in the most-recently-loaded module.
    U32 funcIndex = 0;
    const spacewasm_status_t findStatus = spacewasm_find_export_func(this->m_wasm, value, "main", &funcIndex);

    if (findStatus == SPACEWASM_OK) {
        // Attempt to invoke the main function
        const spacewasm_status_t invokeStatus = spacewasm_invoke(this->m_wasm, value, funcIndex, nullptr, 0);

        // Set the invoke status
        if (invokeStatus == SPACEWASM_OK) {
            this->m_invokeFailed = false;
        } else {
            this->m_invokeFailed = true;
        }
    } else {
        this->m_invokeFailed = true;
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_invokeStartOfLastModule(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    FW_ASSERT(this->m_wasm != nullptr);

    const auto invokeStatus = spacewasm_invoke_start(this->m_wasm, this->m_moduleIndex);
    switch (invokeStatus) {
        case SPACEWASM_RUN_OUT_OF_FUEL:
            this->sequencer_sendSignal_startInvoked();
            break;
        case SPACEWASM_RUN_PAUSE:
            // TODO(tumbar) We need another signal here...
            FW_ASSERT(false);
            break;
        case SPACEWASM_RUN_FINISHED:
            this->sequencer_sendSignal_startFinished();
            break;
        case SPACEWASM_RUN_TRAP:
            this->sequencer_sendSignal_startError();
            break;
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_pendRun(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal,
    const Svc::WasmSequencer_SequenceExecutionArgs& value) {
    // RUN == load the file (unnamed) then invoke its "main".
    this->m_pendingLoad = Svc::WasmSequencer_ModuleLoad(value.get_filePath(), Fw::String(""));
    this->m_hasPendingLoad = true;
    this->m_pendingRun = true;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_pendLoad(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleLoad& value) {
    FW_ASSERT(!this->m_hasPendingLoad);
    this->m_pendingLoad = value;
    this->m_hasPendingLoad = true;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_reportLoadFailure(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO(tumbar): the `load` action does not stash the spacewasm status yet, so
    // we cannot surface the real failure code here.
    this->log_WARNING_HI_ModuleLoadFailed(static_cast<WasmSequencer_SpaceWasmStatus::T>(this->m_loadStatus));
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_reportInvokeFailure(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_load(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    FW_ASSERT(this->m_wasm != nullptr);
    FW_ASSERT(this->m_hasPendingLoad);

    const Fw::String filePath(this->m_pendingLoad.get_fileName());
    const Fw::String moduleName(this->m_pendingLoad.get_moduleName());

    // Acknowledge the pending load
    this->m_hasPendingLoad = false;
    this->m_loadFailed = true;

    Os::File file;
    const Os::File::Status openStatus = file.open(filePath.toChar(), Os::File::OPEN_READ);
    if (openStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileOpenError(filePath, openStatus);
        this->m_loadFailed = true;
        return;
    }

    // Expose the open file to the streaming read trampoline for the duration of
    // this decode.
    this->m_loadFile = &file;

    // A per-load guest linear-memory allocator. Backed by m_guest_pool; released
    // immediately after load (the module retains its own reference).
    spacewasm_allocator_t* alloc =
        spacewasm_allocator_new(&wasmSeqGuestAlloc, &wasmSeqGuestRealloc, &wasmSeqGuestDealloc, this);

    const spacewasm_status_t status = spacewasm_load_module(this->m_wasm, moduleName.toChar(), &this->wasmSeqReadModule,
                                                            this, alloc, &this->m_moduleIndex);

    spacewasm_allocator_destroy(alloc);
    this->m_loadFile = nullptr;
    file.close();

    if (status != SPACEWASM_OK) {
        // TODO(tumbar) We need an EVR here with the load error
        this->m_loadStatus = status;
        this->m_loadFailed = true;
        return;
    }

    // Load succeeded
    this->m_loadFailed = false;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_spin(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    if (this->m_wasm == nullptr) {
        this->sequencer_sendSignal_interpreterTrap(Svc::WasmSequencer_TrapReason::Host);
        return;
    }

    Fw::ParamValid prmValid;
    const auto fuel = this->paramGet_INSTRUCTION_FUEL(prmValid);

    spacewasm_trap_t trap = SPACEWASM_TRAP_NONE;
    const spacewasm_run_status_t runStatus = spacewasm_run(this->m_wasm, fuel, &trap);

    this->m_lastTrap = trap;

    switch (runStatus) {
        case SPACEWASM_RUN_FINISHED:
            this->sequencer_sendSignal_interpreterFinished();
            break;
        case SPACEWASM_RUN_TRAP:
            this->sequencer_sendSignal_interpreterTrap(WasmSequencer::mapTrapReason(trap));
            break;
        case SPACEWASM_RUN_PAUSE:
            this->sequencer_sendSignal_interpreterPause();
            break;
        case SPACEWASM_RUN_OUT_OF_FUEL:
            this->sequencer_sendSignal_interpreterOutOfFuel();
            break;
        default:
            FW_ASSERT(false, runStatus);
            break;
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqSucceeded(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_ACTIVITY_HI_SequenceSucceeded();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqCancelled(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_ACTIVITY_HI_SequenceCancelled();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqFailed(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_WARNING_HI_SequenceFailed();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqTrap(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_WARNING_HI_SequenceTrap(WasmSequencer::mapTrapReason(this->m_lastTrap));
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_report_seqPaused(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_ACTIVITY_HI_SequenceBroken();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendLoadCmdResponse_OK(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    if (this->m_hasPendingLoadCmd) {
        this->cmdResponse_out(this->m_pendingLoadCmd.opCode, this->m_pendingLoadCmd.cmdSeq, Fw::CmdResponse::OK);
        this->m_hasPendingLoadCmd = false;
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendLoadCmdResponse_EXECUTION_ERROR(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    if (this->m_hasPendingLoadCmd) {
        this->cmdResponse_out(this->m_pendingLoadCmd.opCode, this->m_pendingLoadCmd.cmdSeq,
                              Fw::CmdResponse::EXECUTION_ERROR);
        this->m_hasPendingLoadCmd = false;
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendRunCmdResponse_OK(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // Respond to every command blocked on interpreter finish.
    PendingCmd cmd{};
    while (this->m_pendingFinishCmds.dequeue(cmd) == Fw::Success::SUCCESS) {
        this->cmdResponse_out(cmd.opCode, cmd.cmdSeq, Fw::CmdResponse::OK);
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_sendRunCmdResponse_EXECUTION_ERROR(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // Respond to every command blocked on interpreter finish.
    PendingCmd cmd{};
    while (this->m_pendingFinishCmds.dequeue(cmd) == Fw::Success::SUCCESS) {
        this->cmdResponse_out(cmd.opCode, cmd.cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_set_sleepTime(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkStatementTimeout(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
    // check yet. Report "no timeout" so the SM does not wedge waiting on a check.
    this->sequencer_sendSignal_result_checkStatementTimeout_noTimeout();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkShouldWake(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
    this->sequencer_sendSignal_result_checkShouldWake_keepSleeping();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_pendPause(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->m_pendingPause = true;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_clearPause(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->m_pendingPause = false;
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_dispatchPendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    FW_ASSERT(this->m_hasPendingHostFunction);
    this->m_hasPendingHostFunction = false;
    switch (this->m_pendingHostFunction.kind) {
        case PendingHostInvocation::NONE:
            // Invalid host function
            FW_ASSERT(false);
            return;
        case PendingHostInvocation::COMMAND:
            // Dispatch command to CmdDisp
            this->cmdOut_out(0, this->m_pendingHostFunction.buffer, this->m_cmdSeq);
            break;
    }
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_alwaysTrue(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return true;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingRun(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return m_pendingRun;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_invokeFailed(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_invokeFailed;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingPause(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_pendingPause;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_hasPendingHostFunction;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_moduleLoadSucceeded(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return !this->m_loadFailed;
}

}  // namespace Svc
