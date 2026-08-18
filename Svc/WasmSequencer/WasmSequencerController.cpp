// ======================================================================
// \title  WasmSequencerController.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer controller state machine
// ======================================================================

#include "Fw/Cmd/CmdResponseEnumAc.hpp"
#include "Fw/Types/Assert.hpp"
#include "Svc/Seq/BlockStateEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencer_LoadRequestSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleIdxAliasAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_SignalSourceEnumAc.hpp"

namespace Svc {

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_processInvoke(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_InvokeRequest& value) {
    // Resolve the module name to its index within the engine.
    FW_ASSERT(this->m_wasm != nullptr);
    U32 moduleIdx = 0;
    const spacewasm_status_t findStatus =
        spacewasm_find_module(this->m_wasm, value.get_moduleName().toChar(), &moduleIdx);

    if (findStatus != SPACEWASM_OK) {
        this->log_WARNING_LO_ModuleNotFound(value.get_moduleName());
        this->controller_sendSignal_invokeFailed(value.get_context());
    } else {
        // Store the arguments for the ARGS host function round trip and carry the
        // resolved module index forward in the request context.
        this->m_args = value.get_args();
        Svc::WasmSequencer_RequestContext context = value.get_context();
        context.set_moduleIdx(static_cast<WasmSequencer_ModuleIdx>(moduleIdx));
        this->controller_sendSignal_invoked(context);
    }
}

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

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respondInvoke_BUSY(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_InvokeRequest& value) {
    this->log_WARNING_LO_ControllerBusy(value.get_context().get_source(), this->controller_getState());
    this->respondToRequest(value.get_context(), Fw::CmdResponse::BUSY);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respondLoad_BUSY(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_LoadRequest& value) {
    this->log_WARNING_LO_ControllerBusy(value.get_context().get_source(), this->controller_getState());
    this->respondToRequest(value.get_context(), Fw::CmdResponse::BUSY);
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

    // If this concluded an active RUN, report the done to internal callers.
    this->reportSeqDone(value, Fw::CmdResponse::OK);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respond_block_ERROR(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // Respond only to BLOCK requests
    if (value.get_block() == Svc::BlockState::BLOCK) {
        // Respond to this request!
        this->respondToRequest(value, Fw::CmdResponse::EXECUTION_ERROR);
    }

    // Respond to all wait requests
    this->respondToWaiting(Fw::CmdResponse::EXECUTION_ERROR);

    // If this concluded an active RUN, report the done to internal callers.
    this->reportSeqDone(value, Fw::CmdResponse::EXECUTION_ERROR);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_load(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_LoadRequest& value) {
    FW_ASSERT(this->m_wasm != nullptr);

    this->m_args = value.get_args();

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
        this->m_loadFailureStatus = WasmSequencer_Status::ERR_READER_ERROR;
        this->controller_sendSignal_loadFailed(value.get_context());
        return;
    }

    // Expose the open file to the streaming read trampoline for the duration of
    // this decode.
    this->m_loadFile = &file;
    this->m_lastLoadFileName = value.get_fileName();

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
    Svc::WasmSequencer_RequestContext next = value.get_context();

    auto status = spacewasm_load_module(
        this->m_wasm, value.get_moduleName().toChar(),
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
        this->controller_sendSignal_loadFailed(value.get_context());
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
    this->m_tlm.sequencesFailed++;
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

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleStarted(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    this->log_ACTIVITY_HI_SequenceStarting(value.get_moduleIdx());
    if (value.get_source() == Svc::WasmSequencer_SignalSource::COMMAND_RUN ||
        value.get_source() == Svc::WasmSequencer_SignalSource::PORT_RUN) {
        if (this->isConnected_seqStartOut_OutputPort(0)) {
            this->seqStartOut_out(0, this->m_lastLoadFileName, this->m_args);
        }
    }
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
