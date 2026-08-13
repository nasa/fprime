// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

#include "Fw/Cmd/CmdResponseEnumAc.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Os/Mutex.hpp"
#include "Svc/Seq/SeqArgsSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ControllerStateMachine_StateEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleIdxAliasAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleLoadSerializableAc.hpp"
#include "Svc/WasmSequencer/fprime_spacewasm/include/fprime_spacewasm.h"
#include "Svc/WasmSequencer/spacewasm_include/spacewasm.h"
#include "config/FwAssertArgTypeAliasAc.h"

namespace Svc {

// ----------------------------------------------------------------------
// Component construction and destruction
// ----------------------------------------------------------------------

WasmSequencer ::WasmSequencer(const char* const compName)
    : WasmSequencerComponentBase(compName),
      m_page_used_mask(0),
      m_guest_offset(0),
      m_wasm(nullptr),
      m_hasPendingLoadCmd(false),
      m_pendingTimer(),
      m_hasPendingTimer(false),
      m_statementStart(),
      m_hasStatementStart(false),
      m_pendingMainModule(0),
      m_invokeStatus(SPACEWASM_OK),
      m_pendingPause(false),
      m_loadFile(nullptr),
      m_tlmSequencesSucceeded(0),
      m_tlmSequencesFailed(0),
      m_tlmSequencesCancelled(0),
      m_tlmCommandsDispatched(0),
      m_tlmCommandsFailed(0),
      m_sequencesStarted(0),
      m_exitReason(WasmSequencer_ExitReason::UNKNOWN),
      m_exitCode(0),
      m_lastHostFunction(WasmSequencer_HostFunction::NONE),
      m_tlmLastTrapReason(WasmSequencer_TrapReason::NONE),
      m_tlmSequenceName("") {
    getGlobalAllocatorLock()->lock();
    const auto status = spacewasm_fprime_register_global_allocator(
        [](void* userdata, std::size_t size, std::size_t align) -> U8* {
            if (userdata == nullptr) {
                return nullptr;
            }
            return static_cast<WasmSequencer*>(userdata)->globalAlloc(static_cast<U32>(size), static_cast<U32>(align));
        },
        [](void* userdata, U8* ptr, std::size_t size, std::size_t align) {
            (void)size;
            (void)align;
            if (userdata != nullptr) {
                static_cast<WasmSequencer*>(userdata)->globalDealloc(ptr);
            }
        },
        this);
    getGlobalAllocatorLock()->unlock();

    FW_ASSERT(status == SPACEWASM_OK, status);
}

WasmSequencer ::~WasmSequencer() {
    this->destroyStore();

    // Release our slot in the process-wide global-allocator registry so it can
    // be reused by a later sequencer instance.
    getGlobalAllocatorLock()->lock();
    (void)spacewasm_fprime_deregister_global_allocator(this);
    getGlobalAllocatorLock()->unlock();
}

// ----------------------------------------------------------------------
// Handler implementations for typed input ports
// ----------------------------------------------------------------------

void WasmSequencer ::checkTimers_handler(FwIndexType portNum, U32 context) {
    // Drive the sleep-wake / statement-timeout checks in the state machine.
    this->interpreter_sendSignal_checkTimers();
}

void WasmSequencer ::cmdResponseIn_handler(FwIndexType portNum,
                                           FwOpcodeType opCode,
                                           U32 cmdSeq,
                                           const Fw::CmdResponse& response) {
    // The CmdDisp echoes back the context we sent, not a real cmdSeq. We packed
    // our cmdUid into that context (see makeCmdUid); rename for clarity.
    const U32 cmdUid = cmdSeq;
    const U16 sequenceIndex = static_cast<U16>((cmdUid & 0xFFFF0000) >> 16);
    const U16 cmdIndex = static_cast<U16>(cmdUid & 0xFFFF);
    const U16 currentSequenceIndex = static_cast<U16>(this->m_sequencesStarted & 0xFFFF);
    const U16 currentCmdIndex = static_cast<U16>(this->m_tlmCommandsDispatched & 0xFFFF);

    // If the response is from a previous execution window, treat it as a nominal
    // late reply (e.g. a command that returned after a CANCEL) and just report it
    // without failing the current sequence.
    if (sequenceIndex != currentSequenceIndex) {
        this->log_WARNING_LO_CmdResponseFromOldSequence(opCode, response, sequenceIndex, currentSequenceIndex);
        return;
    }

    // From here on the response claims to be from the current sequence, so any
    // inconsistency is a genuine error that should fail the sequence.
    if (this->interpreter_getState() != WasmSequencer_EngineStateMachine_State::RUNNING_AWAITING_RESPONSE_WAITING ||
        this->m_pendingHostFunction.kind != WasmSequencer_HostFunction::COMMAND) {
        this->interpreter_sendSignal_hostResponseUnexpected(WasmSequencer_HostFunction::COMMAND);
        return;
    }

    // Awaiting a command response, but was it for this exact dispatch instance, or
    // an earlier one in this sequence with the same opcode?
    if (cmdIndex != currentCmdIndex) {
        this->log_WARNING_HI_WrongCmdResponseIndex(opCode, response, cmdIndex, currentCmdIndex);
        this->interpreter_sendSignal_hostResponseUnexpected(WasmSequencer_HostFunction::COMMAND);
        return;
    }

    this->m_pendingHostFunction.clear();

    // Track commands that came back with a non-OK response.
    if (response != Fw::CmdResponse::OK) {
        this->m_tlmCommandsFailed++;
    }

    this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(response.e));
}

void WasmSequencer ::writeTelemetry_handler(FwIndexType portNum, U32 context) {
    auto now = this->getTime();

    this->tlmWrite_ControllerState(this->controller_getState(), now);
    this->tlmWrite_EngineState(this->interpreter_getState(), now);
    this->tlmWrite_SequencesSucceeded(this->m_tlmSequencesSucceeded, now);
    this->tlmWrite_SequencesFailed(this->m_tlmSequencesFailed, now);
    this->tlmWrite_SequencesCancelled(this->m_tlmSequencesCancelled, now);
    this->tlmWrite_CommandsDispatched(this->m_tlmCommandsDispatched, now);
    this->tlmWrite_CommandsFailed(this->m_tlmCommandsFailed, now);
    this->tlmWrite_LastTrapReason(this->m_tlmLastTrapReason, now);
    this->tlmWrite_SeqName(this->m_tlmSequenceName, now);
}

// ----------------------------------------------------------------------
// Handler implementations for serial input ports
// ----------------------------------------------------------------------

void WasmSequencer ::serialReply_handler(FwIndexType portNum, Fw::LinearBufferBase& buffer) {
    if (this->interpreter_getState() != WasmSequencer_EngineStateMachine_State::RUNNING_AWAITING_RESPONSE_WAITING ||
        this->m_pendingHostFunction.kind != WasmSequencer_HostFunction::ASYNC_PORT ||
        static_cast<U64>(portNum) != this->m_pendingHostFunction.id) {
        this->interpreter_sendSignal_hostResponseUnexpected(WasmSequencer_HostFunction::ASYNC_PORT);
        return;
    }

    // The guest cannot receive more than it allocated for the return buffer
    if (buffer.getSize() > this->m_pendingHostFunction.len2) {
        this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::ASYNC_PORT, this->m_pendingHostFunction.len2,
                                            static_cast<U32>(buffer.getSize()));
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    // Write the reply payload back into guest memory
    const auto status = spacewasm_mem_write(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr2,
                                            buffer.getBuffAddr(), buffer.getSize());
    if (status != SPACEWASM_OK) {
        this->log_WARNING_HI_HostFunctionInvalidPointer(Svc::WasmSequencer_HostFunction::ASYNC_PORT,
                                                        static_cast<WasmSequencer_Status::T>(status));
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(buffer.getSize()));
}

// ----------------------------------------------------------------------
// Handler implementations for commands
// ----------------------------------------------------------------------

void WasmSequencer ::RUN_cmdHandler(FwOpcodeType opCode,
                                    U32 cmdSeq,
                                    const Fw::CmdStringArg& fileName,
                                    const Svc::BlockState& block,
                                    const SeqArgs& seqArgs) {
    // RUN is only valid from IDLE or READY.
    if (this->controller_getState() != WasmSequencer_ControllerStateMachine_State::IDLE &&
        this->controller_getState() != WasmSequencer_ControllerStateMachine_State::READY) {
        this->log_WARNING_LO_InvalidCommand(this->controller_getState());
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    switch (block) {
        case BlockState::BLOCK: {
            // Queue up a response when the execution finishes
            auto status = this->m_pendingFinishCmds.enqueue(PendingCmd(opCode, cmdSeq));

            // Queuing this command should not fail because the queue should be empty in 'READY'/'IDLE' states
            FW_ASSERT(status == Fw::Success::SUCCESS, status);
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

    this->m_args = seqArgs;
    Fw::String runModuleName = "";
    this->controller_sendSignal_cmd_RUN(WasmSequencer_ModuleLoad(fileName, runModuleName));
}

void WasmSequencer ::WAIT_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    switch (this->controller_getState()) {
        case WasmSequencer_ControllerStateMachine_State::IDLE:
        case WasmSequencer_ControllerStateMachine_State::READY:
            // Nothing is executing, respond immediately
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        default: {
            const auto status = this->m_pendingFinishCmds.enqueue(PendingCmd(opCode, cmdSeq));
            if (status != Fw::Success::SUCCESS) {
                this->log_WARNING_HI_TooManyBlockingCommands();
                this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            }
        }
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
    // Loading is only valid from IDLE or READY.
    if (this->controller_getState() != WasmSequencer_ControllerStateMachine_State::IDLE &&
        this->controller_getState() != WasmSequencer_ControllerStateMachine_State::READY) {
        this->log_WARNING_LO_InvalidCommand(this->controller_getState());
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    FW_ASSERT(!this->m_hasPendingLoadCmd);
    this->m_pendingLoadCmd = PendingCmd(opCode, cmdSeq);
    this->m_hasPendingLoadCmd = true;
    this->controller_sendSignal_cmd_LOAD(Svc::WasmSequencer_ModuleLoad(fileName, name));
}

void WasmSequencer ::INVOKE_cmdHandler(FwOpcodeType opCode,
                                       U32 cmdSeq,
                                       const Fw::CmdStringArg& module,
                                       const Svc::BlockState& block,
                                       const Svc::SeqArgs& seqArgs) {
    // INVOKE is only valid from READY (a module must already be loaded).
    if (this->controller_getState() != WasmSequencer_ControllerStateMachine_State::READY) {
        this->log_WARNING_LO_InvalidCommand(this->controller_getState());
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    // Resolve the module name to its index within the engine.
    FW_ASSERT(this->m_wasm != nullptr);
    U32 moduleIdx = 0;
    const spacewasm_status_t findStatus = spacewasm_find_module(this->m_wasm, module.toChar(), &moduleIdx);
    if (findStatus != SPACEWASM_OK) {
        this->log_WARNING_LO_ModuleNotFound(module);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    switch (block) {
        case BlockState::BLOCK: {
            auto status = this->m_pendingFinishCmds.enqueue(PendingCmd(opCode, cmdSeq));

            // Queuing this command should not fail because the queue should be empty in 'READY' state
            FW_ASSERT(status == Fw::Success::SUCCESS, status);
            break;
        }
        case BlockState::NO_BLOCK:
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        default:
            FW_ASSERT(false, static_cast<FwAssertArgType>(block));
    }

    this->m_args = seqArgs;
    this->controller_sendSignal_cmd_INVOKE(static_cast<Svc::WasmSequencer_ModuleIdx>(moduleIdx));
}

void WasmSequencer ::CANCEL_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->controller_sendSignal_cmd_CANCEL();
    this->interpreter_sendSignal_cmd_CANCEL();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::PAUSE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->interpreter_sendSignal_cmd_PAUSE();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CONTINUE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    switch (this->interpreter_getState()) {
        case WasmSequencer_EngineStateMachine_State::RUNNING_AWAITING_RESPONSE_SLEEPING:
        case WasmSequencer_EngineStateMachine_State::RUNNING_AWAITING_RESPONSE_WAITING:
        case WasmSequencer_EngineStateMachine_State::RUNNING_SPINNING:
            // Already running
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        case WasmSequencer_EngineStateMachine_State::RUNNING_PAUSED:
            this->interpreter_sendSignal_cmd_CONTINUE();
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        default:
            this->log_WARNING_LO_InvalidCommand(this->controller_getState());
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
            break;
    }
}

void WasmSequencer ::takeAllocatorLock() {
    getGlobalAllocatorLock()->lock();

    auto status = spacewasm_fprime_acquire_global_allocator(this);
    FW_ASSERT(status == SPACEWASM_OK, status);
}

void WasmSequencer ::releaseAllocatorLock() {
    auto status = spacewasm_fprime_release_global_allocator(this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    getGlobalAllocatorLock()->unlock();
}

}  // namespace Svc
