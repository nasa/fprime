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
#include "Svc/Seq/BlockStateEnumAc.hpp"
#include "Svc/Seq/SeqArgsSerializableAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ControllerStateMachine_StateEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_SignalSourceEnumAc.hpp"
#include "Svc/WasmSequencer/fprime_spacewasm/include/fprime_spacewasm.h"
#include "Svc/WasmSequencer/spacewasm_include/spacewasm.h"

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
      m_seqRunActive(false),
      m_pendingTimer(),
      m_hasPendingTimer(false),
      m_statementStart(),
      m_hasStatementStart(false),
      m_pendingMainModule(0),
      m_invokeStatus(SPACEWASM_OK),
      m_pendingPause(false),
      m_loadFile(nullptr),
      m_sequencesStarted(0) {
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
    const U16 currentCmdIndex = static_cast<U16>(this->m_tlm.commandsDispatched & 0xFFFF);

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
        this->m_tlm.commandsFailed++;
    }

    this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(response.e));
}

void WasmSequencer ::writeTelemetry_handler(FwIndexType portNum, U32 context) {
    auto now = this->getTime();

    this->tlmWrite_ControllerState(this->controller_getState(), now);
    this->tlmWrite_EngineState(this->interpreter_getState(), now);
    this->tlmWrite_SequencesSucceeded(this->m_tlm.sequencesSucceeded, now);
    this->tlmWrite_SequencesFailed(this->m_tlm.sequencesFailed, now);
    this->tlmWrite_SequencesCancelled(this->m_tlm.sequencesCancelled, now);
    this->tlmWrite_CommandsDispatched(this->m_tlm.commandsDispatched, now);
    this->tlmWrite_CommandsFailed(this->m_tlm.commandsFailed, now);
    this->tlmWrite_LastTrapReason(this->m_exit.lastTrapReason, now);
    this->tlmWrite_SeqName(this->m_tlm.sequenceName, now);
}

void WasmSequencer ::seqRunIn_handler(FwIndexType portNum, const Fw::StringBase& filename, const Svc::SeqArgs& args) {
    if (this->m_hasPendingLoad) {
        // We are waiting for another load request to be processed by the state machine
        // We should reject this command since it would failed with "NOT_ALLOWED" once it enters the state machine
        // TODO(tumbar) Change the response to BUSY and the EVR to signal kind
        this->log_WARNING_LO_InvalidCommand(Svc::WasmSequencer_ControllerStateMachine_State::LOADING_TO_RUN);
        return;
    }

    this->m_args = args;
    this->m_pendingLoad.fileName = filename;
    this->m_pendingLoad.moduleName = "";
    this->m_hasPendingLoad = true;

    Fw::String runModuleName = "";

    this->controller_sendSignal_run(Svc::WasmSequencer_RequestContext(
        WasmSequencer_SignalSource::PORT_RUN, 0, 0, BlockState::NO_BLOCK,
        /* moduleIdx */ 0  // placeholder, gets filled in after load
        ));
}

void WasmSequencer ::seqCancelIn_handler(FwIndexType portNum) {
    // Port-driven CANCEL. The only state a sequence cannot be cancelled from is
    // IDLE (nothing is running). Mirrors the CANCEL command otherwise, but has no
    // command response to send.
    if (this->controller_getState() == WasmSequencer_ControllerStateMachine_State::IDLE) {
        this->log_WARNING_HI_InvalidSeqCancelCall(this->controller_getState());
        return;
    }

    this->controller_sendSignal_cancel();
    this->interpreter_sendSignal_cmd_CANCEL();
}

// ----------------------------------------------------------------------
// Handler implementations for serial input ports
// ----------------------------------------------------------------------

void WasmSequencer ::serialReply_handler(FwIndexType portNum, Fw::LinearBufferBase& buffer) {
    if (this->interpreter_getState() != WasmSequencer_EngineStateMachine_State::RUNNING_AWAITING_RESPONSE_WAITING ||
        this->m_pendingHostFunction.kind != WasmSequencer_HostFunction::ASYNC_PORT ||
        static_cast<U32>(portNum) != this->m_pendingHostFunction.u.asyncPort.index) {
        this->interpreter_sendSignal_hostResponseUnexpected(WasmSequencer_HostFunction::ASYNC_PORT);
        return;
    }

    // The guest cannot receive more than it allocated for the return buffer
    if (buffer.getSize() > this->m_pendingHostFunction.u.asyncPort.returnLen) {
        this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::ASYNC_PORT,
                                            this->m_pendingHostFunction.u.asyncPort.returnLen,
                                            static_cast<U32>(buffer.getSize()));
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    // Write the reply payload back into guest memory
    const auto status =
        spacewasm_mem_write(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.u.asyncPort.returnPtr,
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
    if (this->m_hasPendingLoad) {
        // We are waiting for another load request to be processed by the state machine
        // We should reject this command since it would failed with "NOT_ALLOWED" once it enters the state machine
        // TODO(tumbar) Change EVR to signal kind
        this->log_WARNING_LO_InvalidCommand(Svc::WasmSequencer_ControllerStateMachine_State::LOADING_TO_RUN);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
        return;
    }

    this->m_args = seqArgs;
    this->m_pendingLoad.fileName = fileName;
    this->m_pendingLoad.moduleName = "";
    this->m_hasPendingLoad = true;

    this->controller_sendSignal_run(Svc::WasmSequencer_RequestContext(
        WasmSequencer_SignalSource::COMMAND_RUN, opCode, cmdSeq, block,
        /* moduleIdx */ 0  // placeholder, gets filled in after load
        ));
}

void WasmSequencer ::WAIT_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    switch (this->controller_getState()) {
        case WasmSequencer_ControllerStateMachine_State::IDLE:
        case WasmSequencer_ControllerStateMachine_State::READY:
            // Nothing is executing, respond immediately
            this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
            break;
        default: {
            const auto status = this->m_waiting.enqueue(WaitingCmd(opCode, cmdSeq));
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
    if (this->m_hasPendingLoad) {
        // We are waiting for another load request to be processed by the state machine
        // We should reject this command since it would failed with "NOT_ALLOWED" once it enters the state machine
        // TODO(tumbar) Change EVR to signal kind
        this->log_WARNING_LO_InvalidCommand(Svc::WasmSequencer_ControllerStateMachine_State::LOADING_TO_RUN);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::BUSY);
        return;
    }

    this->m_pendingLoad.fileName = fileName;
    this->m_pendingLoad.moduleName = "";
    this->m_hasPendingLoad = true;

    this->controller_sendSignal_load(Svc::WasmSequencer_RequestContext(
        WasmSequencer_SignalSource::COMMAND_RUN, opCode, cmdSeq, Svc::BlockState::NO_BLOCK,
        /* moduleIdx */ 0  // placeholder, gets filled in after load
        ));
}

void WasmSequencer ::INVOKE_cmdHandler(FwOpcodeType opCode,
                                       U32 cmdSeq,
                                       const Fw::CmdStringArg& module,
                                       const Svc::BlockState& block,
                                       const Svc::SeqArgs& seqArgs) {
    // Resolve the module name to its index within the engine.
    FW_ASSERT(this->m_wasm != nullptr);
    U32 moduleIdx = 0;
    const spacewasm_status_t findStatus = spacewasm_find_module(this->m_wasm, module.toChar(), &moduleIdx);
    if (findStatus != SPACEWASM_OK) {
        this->log_WARNING_LO_ModuleNotFound(module);
        this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::EXECUTION_ERROR);
        return;
    }

    // TODO(tumbar) This unconditionally overwrites the args, we should gate these with a pend bool...
    this->m_args = seqArgs;

    this->controller_sendSignal_invoke(Svc::WasmSequencer_RequestContext(
        WasmSequencer_SignalSource::COMMAND_INVOKE, opCode, cmdSeq, block,
        /* moduleIdx */ 0  // placeholder, gets filled in after load
        ));
}

void WasmSequencer ::CANCEL_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->controller_sendSignal_cancel();
    this->interpreter_sendSignal_cmd_CANCEL();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::PAUSE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->m_pendingPause = true;
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
