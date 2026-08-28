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

    // Clear any pending waits that locked during load
    if (value.get_source() == Svc::WasmSequencer_SignalSource::COMMAND_LOAD) {
        this->respondToWaiting(Fw::CmdResponse::OK);
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

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_incrementSequenceFailure(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    this->m_tlm.sequencesFailed++;
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_setCancelRequested(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    this->m_cancelRequested = true;
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_clearCancelRequested(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) {
    this->m_cancelRequested = false;
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_cancelPendingRequest(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    this->m_tlm.sequencesCancelled++;
    this->respondToRequest(value, Fw::CmdResponse::EXECUTION_ERROR);
    this->respondToWaiting(Fw::CmdResponse::EXECUTION_ERROR);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respondInvoke_BUSY(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_InvokeRequest& value) {
    this->log_WARNING_LO_ControllerBusy(value.get_context().get_source(), this->controller_getState());
    this->respondToRequest(value.get_context(), Fw::CmdResponse::BUSY);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_respondInvoke_ERROR(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_InvokeRequest& value) {
    this->log_WARNING_LO_ControllerCannotInvoke();
    this->respondToRequest(value.get_context(), Fw::CmdResponse::EXECUTION_ERROR);
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

bool WasmSequencer ::resolveSequencePath(const Fw::StringBase& fileName, Fw::String& filePath) {
    // Resolve the requested path against the SEQ_BASE_DIR parameter
    Fw::ParamValid baseDirValid;
    const Fw::ParamString baseDir = this->paramGet_SEQ_BASE_DIR(baseDirValid);

    if (baseDir.length() == 0) {
        filePath = fileName;
        return true;
    }

    // With a base dir configured, SEQ_BASE_DIR acts as a containment boundary.
    // A ground-supplied file name containing a ".." component could escape it
    // (e.g. "../../etc/passwd"), so reject such names rather than opening a path
    // outside the configured base directory.
    if (WasmSequencer::pathHasParentTraversal(fileName)) {
        this->log_WARNING_HI_SequenceFilePathNotContained(baseDir, fileName);
        return false;
    }

    // Join the base dir and file name with exactly one '/' separator. Without an
    // explicit separator a base dir that lacks a trailing slash both mis-resolves
    // ordinary names ("seqs" + "a.wasm" -> "seqsa.wasm") and can escape the
    // containment boundary ("seqs" + "_priv/x.wasm" -> "seqs_priv/x.wasm", a sibling
    // directory). The ".." rejection above plus a guaranteed base-dir prefix keeps
    // the resolved path inside the configured directory.
    const FwSizeType baseLen = static_cast<FwSizeType>(baseDir.length());
    const char* const separator = (baseLen > 0 && baseDir.toChar()[baseLen - 1] == '/') ? "" : "/";

    const Fw::FormatStatus fmtStatus = filePath.format("%s%s%s", baseDir.toChar(), separator, fileName.toChar());
    if (fmtStatus != Fw::FormatStatus::SUCCESS) {
        FW_ASSERT(fmtStatus == Fw::FormatStatus::OVERFLOWED, static_cast<FwAssertArgType>(fmtStatus));
        this->log_WARNING_HI_SequenceFilePathTooLong(baseDir, fileName);
        return false;
    }

    return true;
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_load(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_LoadRequest& value) {
    FW_ASSERT(this->m_wasm != nullptr);

    this->m_args = value.get_args();

    // Resolve the sequence file path against SEQ_BASE_DIR
    Fw::String filePath;
    if (!this->resolveSequencePath(value.get_fileName(), filePath)) {
        this->controller_sendSignal_loadFailed(value.get_context());
        return;
    }

    // Record the sequence name for telemetry (module name, or filename stem).
    this->setSequenceName(filePath, value.get_moduleName());

    Os::File file;
    const Os::File::Status openStatus = file.open(filePath.toChar(), Os::File::OPEN_READ);
    if (openStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileOpenError(filePath, openStatus);
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
    spacewasm_allocator_t* alloc =
        spacewasm_allocator_new(&WasmSequencer::guestAllocCallback, &WasmSequencer::guestReallocCallback,
                                &WasmSequencer::guestDeallocCallback, /* userdata */ this);
    FW_ASSERT(alloc != nullptr);

    U32 moduleIndex = 0;
    Svc::WasmSequencer_RequestContext next = value.get_context();

    auto status = spacewasm_load_module(this->m_wasm, value.get_moduleName().toChar(),
                                        &WasmSequencer::readModuleChunkCallback, this, alloc, &moduleIndex);

    spacewasm_allocator_destroy(alloc);
    next.set_moduleIdx(static_cast<WasmSequencer_ModuleIdx>(moduleIndex));

    this->releaseAllocatorLock();

    this->m_loadFile = nullptr;
    file.close();

    if (status == SPACEWASM_OK) {
        this->controller_sendSignal_loadSucceeded(next);
    } else {
        this->log_WARNING_HI_ModuleLoadFailed(WasmSequencer_Status(status));
        this->controller_sendSignal_loadFailed(value.get_context());
    }
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
    // Get the failure status of why the main module is invalid
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

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_runEngine(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    this->interpreter_sendSignal_run(value);
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
    // A failure while running a module's start function (LOAD-with-start, or the
    // start phase of a RUN-with-start).
    this->reportSequenceRuntimeFailure(value.get_moduleIdx(), WasmSequencer_SequencePhase::START);
}

void WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_action_reportModuleMainFailed(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    // A failure while running a module's main function.
    this->reportSequenceRuntimeFailure(value.get_moduleIdx(), WasmSequencer_SequencePhase::MAIN);
}

void WasmSequencer ::reportSequenceRuntimeFailure(WasmSequencer_ModuleIdx moduleIdx,
                                                  WasmSequencer_SequencePhase phase) {
    switch (this->m_exit.reason) {
        case WasmSequencer_ExitReason::CANCEL:
            this->m_tlm.sequencesCancelled++;
            this->log_ACTIVITY_HI_SequenceCancelled(moduleIdx, phase);
            break;
        case WasmSequencer_ExitReason::HOST_PANIC:
            this->m_tlm.sequencesFailed++;
            this->log_WARNING_HI_SequencePanic(moduleIdx, phase, this->m_exit.code);
            break;
        case WasmSequencer_ExitReason::INTERPRETER_TRAP:
            this->m_tlm.sequencesFailed++;
            this->log_WARNING_HI_SequenceTrapped(moduleIdx, phase, this->m_exit.lastTrapReason);
            break;
        case WasmSequencer_ExitReason::INTERPRETER_FINISHED:
        case WasmSequencer_ExitReason::HOST_EXIT:
            // Only reached on the failure path: a non-zero main return value or a
            // non-zero fprime.exit code.
            this->m_tlm.sequencesFailed++;
            this->log_WARNING_HI_SequenceExited(moduleIdx, phase, this->m_exit.code);
            break;
        case WasmSequencer_ExitReason::UNKNOWN:
        case WasmSequencer_ExitReason::REPLY_TIMEOUT:
        case WasmSequencer_ExitReason::HOST_FAILURE:
        case WasmSequencer_ExitReason::UNEXPECTED_REPLY:
        case WasmSequencer_ExitReason::TIMER_INCOMPARABLE:
        default:
            this->m_tlm.sequencesFailed++;
            this->log_WARNING_HI_SequenceHostFailure(moduleIdx, phase, this->m_exit.reason,
                                                     this->m_exit.lastHostFunction);
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
            return false;
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
    // The engine sets m_exit.reason and m_exit.code before signalling
    // engineFinished. A run succeeds only when it finished with a zero code:
    // main returned 0 (a void return reports code 0) or the guest called
    // fprime.exit(0)
    switch (this->m_exit.reason) {
        case WasmSequencer_ExitReason::INTERPRETER_FINISHED:
        case WasmSequencer_ExitReason::HOST_EXIT:
            return this->m_exit.code == 0;
        default:
            return false;
    }
}

bool WasmSequencer ::Svc_WasmSequencer_ControllerStateMachine_guard_cancelRequested(
    SmId smId,
    Svc_WasmSequencer_ControllerStateMachine::Signal signal) const {
    return this->m_cancelRequested;
}

}  // namespace Svc
