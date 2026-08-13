// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Fw/Types/Assert.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ExitReasonEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleIdxAliasAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_StatusEnumAc.hpp"
#include "spacewasm.h"

namespace Svc {

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_loadCmd_OK(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    if (this->m_hasPendingLoadCmd) {
        this->cmdResponse_out(this->m_pendingLoadCmd.opCode, this->m_pendingLoadCmd.cmdSeq, Fw::CmdResponse::OK);
        this->m_hasPendingLoadCmd = false;
    }
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_loadCmd_ERROR(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    if (this->m_hasPendingLoadCmd) {
        this->cmdResponse_out(this->m_pendingLoadCmd.opCode, this->m_pendingLoadCmd.cmdSeq,
                              Fw::CmdResponse::EXECUTION_ERROR);
        this->m_hasPendingLoadCmd = false;
    }
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_runCmd_OK(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    // Respond to every command blocked on interpreter finish. Sequence-level
    // telemetry and events are owned by reportModuleSucceeded/reportModuleFailed
    // (which fire on all engine-completion paths, not just RUN commands).
    PendingCmd cmd{};
    while (this->m_pendingFinishCmds.dequeue(cmd) == Fw::Success::SUCCESS) {
        this->cmdResponse_out(cmd.opCode, cmd.cmdSeq, Fw::CmdResponse::OK);
    }
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_runCmd_ERROR(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    // Respond to every command blocked on interpreter finish. Sequence-level
    // telemetry and events are owned by reportModuleFailed.
    PendingCmd cmd{};
    while (this->m_pendingFinishCmds.dequeue(cmd) == Fw::Success::SUCCESS) {
        this->cmdResponse_out(cmd.opCode, cmd.cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
    }
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleSucceeded(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    // Reached only when a module's main function ran to completion successfully.
    // A successful load/start alone does not count as a sequence success.
    this->m_tlmSequencesSucceeded++;
    this->log_ACTIVITY_HI_SequenceSucceeded(this->m_invokedModule);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    // Reached on any engine-completion failure, whether the engine was running a
    // start function (LOAD/RUN) or a main function (RUN). Reason about the cause
    // so the right counter is bumped.
    switch (this->m_exitReason) {
        case WasmSequencer_ExitReason::CANCEL:
            this->m_tlmSequencesCancelled++;
            this->log_ACTIVITY_HI_SequenceCancelled(this->m_invokedModule);
            break;
        default:
            this->m_tlmSequencesFailed++;
            this->log_WARNING_HI_SequenceFailed(this->m_invokedModule, this->m_exitReason, this->m_exitCode,
                                                 this->m_tlmLastTrapReason, this->m_failedHostFunction);
            break;
    }
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_load(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleLoad& value) {
    // Resolve the requested path against the SEQ_BASE_DIR parameter. An empty
    // base dir (the default) means paths are used verbatim; otherwise a single
    // '/' is inserted between the base dir and the requested path, matching the
    // parameter's documented contract.
    Fw::ParamValid baseDirValid;
    const Fw::ParamString baseDir = this->paramGet_SEQ_BASE_DIR(baseDirValid);
    Fw::String filePath;
    if (baseDir.length() == 0) {
        filePath = value.get_fileName();
    } else {
        // The result is truncated to filePath's capacity on overflow; a
        // truncated path will then fail to open and report FileOpenError.
        const Fw::FormatStatus fmtStatus = filePath.format("%s/%s", baseDir.toChar(), value.get_fileName().toChar());
        if (fmtStatus != Fw::FormatStatus::SUCCESS) {
            FW_ASSERT(fmtStatus == Fw::FormatStatus::OVERFLOWED, static_cast<FwAssertArgType>(fmtStatus));
            this->log_WARNING_HI_SequenceFilePathTooLong(baseDir, value.get_fileName());
        }
    }

    // Record the sequence name for telemetry (module name, or filename stem).
    this->setSequenceName(filePath, value.get_moduleName());

    Os::File file;
    const Os::File::Status openStatus = file.open(filePath.toChar(), Os::File::OPEN_READ);
    if (openStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileOpenError(filePath, openStatus);
        this->controller_sendSignal_loadFailed(WasmSequencer_Status::ERR_READER_ERROR);
        return;
    }

    // Expose the open file to the streaming read trampoline for the duration of
    // this decode.
    this->m_loadFile = &file;

    this->takeAllocatorLock();

    // A per-load guest linear-memory allocator. Backed by m_guest_pool; released
    // immediately after load (the module retains its own reference).
    spacewasm_allocator_t* alloc = spacewasm_allocator_new(
        /* alloc */
        [](void* userdata, const size_t size, const size_t align) -> U8* {
            FW_ASSERT(userdata != nullptr);
            return static_cast<WasmSequencer*>(userdata)->guestAlloc(static_cast<U32>(size), static_cast<U32>(align));
        },
        /* realloc */
        [](void* userdata, uint8_t* ptr, size_t old_size, size_t new_size, size_t align) -> U8* {
            (void)userdata;
            (void)ptr;
            (void)old_size;
            (void)new_size;
            (void)align;

            // We turn off memory.grow so this should never be called!
            // This nullptr will bubble as a reallocation failure
            return nullptr;
        },
        /* dealloc */
        [](void* userdata, U8* ptr, size_t size, size_t align) {
            FW_ASSERT(userdata != nullptr);
            (void)align;
            static_cast<WasmSequencer*>(userdata)->guestDealloc(ptr, static_cast<U32>(size));
        },
        /* userdata */ this);

    U32 moduleIndex;

    auto status = spacewasm_load_module(
        this->m_wasm, value.get_moduleName().toChar(),
        [](void* userdata, const U8** outBuf, std::size_t* outLen) -> spacewasm_read_result_t {
            FW_ASSERT(userdata != nullptr);
            return static_cast<WasmSequencer*>(userdata)->readModuleChunk(outBuf, outLen);
        },
        this, alloc, &moduleIndex);

    spacewasm_allocator_destroy(alloc);

    this->releaseAllocatorLock();

    this->m_loadFile = nullptr;
    file.close();

    if (status == SPACEWASM_OK) {
        this->controller_sendSignal_loadSucceeded(static_cast<WasmSequencer_ModuleIdx>(moduleIndex));
    } else {
        this->controller_sendSignal_loadFailed(WasmSequencer_Status(status));
    }
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleLoadFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_Status& value) {
    this->log_WARNING_HI_ModuleLoadFailed(value);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_invokeStart(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleIdx& value) {
    this->m_pendingMainModule = value;

    U32 start_module_idx;
    U32 start_function_idx;

    // Get the module's start function WasmRef
    auto status = spacewasm_module_start(this->m_wasm, static_cast<U32>(value), &start_module_idx, &start_function_idx);
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->m_invokedModule = static_cast<WasmSequencer_ModuleIdx>(start_module_idx);
    this->m_invokeStatus = spacewasm_invoke(this->m_wasm, start_module_idx, start_function_idx, nullptr, 0);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_invokeMain(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleIdx& value) {
    this->invokeMainOnModule(value);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_invokeMainPending(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    // Start function has finished; run main on the module we loaded (invokeStart
    // may have left m_invokedModule pointing at a different module that owned the
    // start function).
    this->invokeMainOnModule(this->m_pendingMainModule);
}

void WasmSequencer ::invokeMainOnModule(WasmSequencer_ModuleIdx moduleIdx) {
    // Resolve the main function of given module index
    U32 funcIndex = 0;
    auto status = spacewasm_find_export_func(this->m_wasm, static_cast<U32>(moduleIdx), "main", &funcIndex);

    // This should always succeed because our state machine is checking whether this module has a main/is-valid
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->m_invokedModule = moduleIdx;
    this->m_invokeStatus = spacewasm_invoke(this->m_wasm, static_cast<U32>(moduleIdx), funcIndex, nullptr, 0);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleInvalidMain(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleIdx& value) {
    // A module that cannot be run (no valid main) counts as a failed sequence.
    this->m_tlmSequencesFailed++;
    auto problemStatus = this->validateModuleMain(value);
    this->log_WARNING_HI_InvalidModuleEntrypoint(value, WasmSequencer_Status(problemStatus));
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleMainInvokeFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    this->log_WARNING_HI_ModuleMainInvokeFailed(WasmSequencer_Status(this->m_invokeStatus));
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStartInvokeFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    this->log_WARNING_HI_ModuleStartInvokeFailed(WasmSequencer_Status(this->m_invokeStatus));
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_resetStore(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    this->createStore();
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_runEngine(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    this->interpreter_sendSignal_run();
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_guard_moduleHasStart(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleIdx& value) const {
    U32 start_module_idx;
    U32 start_function_idx;
    auto status = spacewasm_module_start(this->m_wasm, static_cast<U32>(value), &start_module_idx, &start_function_idx);

    switch (status) {
        case SPACEWASM_OK:
            return true;
        case SPACEWASM_ERR_NOT_FOUND:
            return false;
        default:
            // All other status means we passed the c api invalid options
            FW_ASSERT(false, status);
            break;
    }
}

bool WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_guard_moduleHasValidMain(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_ModuleIdx& value) const {
    return this->validateModuleMain(value) == SPACEWASM_OK;
}

bool WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_guard_invokeSucceeded(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) const {
    return this->m_invokeStatus == SPACEWASM_OK;
}

bool WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_guard_interpreterSucceeded(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) const {
    // The engine sets m_exitReason before signalling engineFinished. A run
    // succeeds when the interpreter finished normally, or when the guest called
    // fprime.exit(0) (a clean, explicit termination). Every other exit reason
    // (trap, non-zero exit, panic, timeout, host failure, unexpected reply,
    // cancel) is a failure.
    switch (this->m_exitReason) {
        case WasmSequencer_ExitReason::INTERPRETER_FINISHED:
            return true;
        case WasmSequencer_ExitReason::HOST_EXIT:
            return this->m_exitCode == 0;
        default:
            return false;
    }
}

}  // namespace Svc
