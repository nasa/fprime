// ======================================================================
// \title  WasmSequencerController.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer controller state machine
// ======================================================================

#include "Fw/Cmd/CmdResponseEnumAc.hpp"
#include "Fw/Types/Assert.hpp"
#include "Svc/Seq/BlockStateEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleIdxAliasAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respond_noblock_OK(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // Respond only to NO_BLOCK requests
    if (value.get_block() == Svc::BlockState::NO_BLOCK) {
        // Respond to this request!
        this->respondToRequest(value, Fw::CmdResponse::OK);
    }
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respond_ERROR(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    this->respondToRequest(value, Fw::CmdResponse::EXECUTION_ERROR);

    // Respond to all wait requests
    this->respondToWaiting(Fw::CmdResponse::EXECUTION_ERROR);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respond_NOT_ALLOWED(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // TODO(tumbar) Change the response to BUSY and the EVR to signal kind
    this->log_WARNING_LO_InvalidCommand(Svc::WasmSequencer_ControllerStateMachine_State::LOADING_TO_RUN);
    this->respondToRequest(value, Fw::CmdResponse::EXECUTION_ERROR);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respond_block_OK(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // Respond only to BLOCK requests
    if (value.get_block() == Svc::BlockState::BLOCK) {
        // Respond to this request!
        this->respondToRequest(value, Fw::CmdResponse::OK);
    }

    // Respond to all wait requests
    this->respondToWaiting(Fw::CmdResponse::OK);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respond_block_ERROR(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_load(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    FW_ASSERT(this->m_hasPendingLoad);

    // We are processing this load
    this->m_hasPendingLoad = false;

    // Resolve the requested path against the SEQ_BASE_DIR parameter. An empty
    // base dir (the default) means paths are used verbatim; otherwise a single
    // '/' is inserted between the base dir and the requested path, matching the
    // parameter's documented contract.
    Fw::ParamValid baseDirValid;
    const Fw::ParamString baseDir = this->paramGet_SEQ_BASE_DIR(baseDirValid);
    Fw::String filePath;
    if (baseDir.length() == 0) {
        filePath = this->m_pendingLoad.fileName;
    } else {
        // The result is truncated to filePath's capacity on overflow; a
        // truncated path will then fail to open and report FileOpenError.
        const Fw::FormatStatus fmtStatus =
            filePath.format("%s/%s", baseDir.toChar(), this->m_pendingLoad.fileName.toChar());
        if (fmtStatus != Fw::FormatStatus::SUCCESS) {
            FW_ASSERT(fmtStatus == Fw::FormatStatus::OVERFLOWED, static_cast<FwAssertArgType>(fmtStatus));
            this->log_WARNING_HI_SequenceFilePathTooLong(baseDir, this->m_pendingLoad.fileName);
        }
    }

    // Record the sequence name for telemetry (module name, or filename stem).
    this->setSequenceName(filePath, this->m_pendingLoad.moduleName);

    Os::File file;
    const Os::File::Status openStatus = file.open(filePath.toChar(), Os::File::OPEN_READ);
    if (openStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileOpenError(filePath, openStatus);
        this->m_loadFailureStatus = WasmSequencer_Status::ERR_READER_ERROR;
        this->controller_sendSignal_loadFailed(value);
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
    Svc::WasmSequencer_RequestContext next = value;

    auto status = spacewasm_load_module(
        this->m_wasm, this->m_pendingLoad.moduleName.toChar(),
        [](void* userdata, const U8** outBuf, std::size_t* outLen) -> spacewasm_read_result_t {
            FW_ASSERT(userdata != nullptr);
            return static_cast<WasmSequencer*>(userdata)->readModuleChunk(outBuf, outLen);
        },
        this, alloc, &moduleIndex);

    spacewasm_allocator_destroy(alloc);
    next.set_moduleIdx(static_cast<WasmSequencer_ModuleIdx>(moduleIndex));

    this->releaseAllocatorLock();

    this->m_loadFile = nullptr;
    file.close();

    if (status == SPACEWASM_OK) {
        this->controller_sendSignal_loadSucceeded(next);
    } else {
        this->m_loadFailureStatus = WasmSequencer_Status(status);
        this->controller_sendSignal_loadFailed(value);
    }
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleLoadFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    // A failed load counts as a failed sequence: the RUN/LOAD attempt did not
    // reach a runnable module.
    this->m_tlm.sequencesFailed++;
    this->log_WARNING_HI_ModuleLoadFailed(this->m_loadFailureStatus);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_invokeStart(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    U32 start_module_idx;
    U32 start_function_idx;

    // Get the module's start function WasmRef
    auto status = spacewasm_module_start(this->m_wasm, value.get_moduleIdx(), &start_module_idx, &start_function_idx);
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->m_invokeStatus = spacewasm_invoke(this->m_wasm, start_module_idx, start_function_idx, nullptr, 0);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_invokeMain(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // Resolve the main function of given module index
    U32 funcIndex = 0;
    auto status = spacewasm_find_export_func(this->m_wasm, static_cast<U32>(value.get_moduleIdx()), "main", &funcIndex);

    // This should always succeed because our state machine is checking whether this module has a main/is-valid
    FW_ASSERT(status == SPACEWASM_OK, status);

    this->m_invokeStatus =
        spacewasm_invoke(this->m_wasm, static_cast<U32>(value.get_moduleIdx()), funcIndex, nullptr, 0);
}
void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleInvalidMain(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // A module that cannot be run (no valid main) counts as a failed sequence.
    auto problemStatus = this->validateModuleMain(value.get_moduleIdx());
    this->log_WARNING_HI_InvalidModuleEntrypoint(value.get_moduleIdx(), WasmSequencer_Status(problemStatus));
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleMainInvokeFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    this->log_WARNING_HI_ModuleMainInvokeFailed(WasmSequencer_Status(this->m_invokeStatus));
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStartInvokeFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    this->log_WARNING_HI_ModuleStartInvokeFailed(WasmSequencer_Status(this->m_invokeStatus));
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_resetStore(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    this->createStore();
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_setContext(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    FW_ASSERT(!this->m_hasExectingContext);
    this->m_hasExectingContext = true;
    this->m_executingContext = value;
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_clearContext(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    FW_ASSERT(this->m_hasExectingContext);
    this->m_hasExectingContext = false;
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_runEngine(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    this->interpreter_sendSignal_run();
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleSucceeded(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // Reached only when a module's main function ran to completion successfully.
    // A successful load/start alone does not count as a sequence success.
    this->m_tlm.sequencesSucceeded++;
    this->log_ACTIVITY_HI_SequenceSucceeded(value.get_moduleIdx());
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStartFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // Reached on any engine-completion failure, whether the engine was running a
    // start function (LOAD/RUN) or a main function (RUN). Reason about the cause
    // so the right counter is bumped.
    switch (this->m_exit.reason) {
        case WasmSequencer_ExitReason::CANCEL:
            this->m_tlm.sequencesCancelled++;
            this->log_ACTIVITY_HI_SequenceCancelled(value.get_moduleIdx());
            break;
        default:
            this->m_tlm.sequencesFailed++;
            this->log_WARNING_HI_SequenceStartFailed(value.get_moduleIdx(), this->m_exit.reason, this->m_exit.code,
                                                     this->m_exit.lastTrapReason, this->m_exit.lastHostFunction);
            break;
    }
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // Reached on any engine-completion failure, whether the engine was running a
    // start function (LOAD/RUN) or a main function (RUN). Reason about the cause
    // so the right counter is bumped.
    switch (this->m_exit.reason) {
        case WasmSequencer_ExitReason::CANCEL:
            this->m_tlm.sequencesCancelled++;
            this->log_ACTIVITY_HI_SequenceCancelled(value.get_moduleIdx());
            break;
        default:
            this->m_tlm.sequencesFailed++;
            this->log_WARNING_HI_SequenceFailed(value.get_moduleIdx(), this->m_exit.reason, this->m_exit.code,
                                                this->m_exit.lastTrapReason, this->m_exit.lastHostFunction);
            break;
    }
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_guard_moduleHasStart(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) const {
    U32 start_module_idx;
    U32 start_function_idx;
    auto status = spacewasm_module_start(this->m_wasm, static_cast<U32>(value.get_moduleIdx()), &start_module_idx,
                                         &start_function_idx);

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
    const Svc::WasmSequencer_RequestContext& value) const {
    return this->validateModuleMain(value.get_moduleIdx()) == SPACEWASM_OK;
}

bool WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_guard_invokeSucceeded(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) const {
    return this->m_invokeStatus == SPACEWASM_OK;
}

bool WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_guard_interpreterSucceeded(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) const {
    // The engine sets m_exit.reason before signalling engineFinished. A run
    // succeeds when the interpreter finished normally, or when the guest called
    // fprime.exit(0) (a clean, explicit termination). Every other exit reason
    // (trap, non-zero exit, panic, timeout, host failure, unexpected reply,
    // cancel) is a failure.
    switch (this->m_exit.reason) {
        case WasmSequencer_ExitReason::INTERPRETER_FINISHED:
            return true;
        case WasmSequencer_ExitReason::HOST_EXIT:
            return this->m_exit.code == 0;
        default:
            return false;
    }
}

}  // namespace Svc
