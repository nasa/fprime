// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Svc/WasmSequencer/WasmSequencer.hpp"

#include "Fw/Cmd/CmdResponseEnumAc.hpp"
#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/LinearBufferTemplate.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Os/File.hpp"
#include "Svc/WasmSequencer/WasmSequencer_ModuleIdxAliasAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_SequencerStateMachine_StateEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_TrapReasonEnumAc.hpp"
#include "config/FwAssertArgTypeAliasAc.h"
#include "spacewasm.h"

namespace Svc {

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
    // Install the process-wide global page allocator, backed by this instance's
    // page pool. The store/bytecode live entirely within m_memory_pool.
    // TODO(tumbar) We need to figure out a thread-local or context sensitive way to allow multiple sequencers...
    const I32 status = spacewasm_set_global_allocator(
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
    FW_ASSERT(status == 0, status);
}

WasmSequencer ::~WasmSequencer() {
    this->destroyStore();
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
    } else if (this->m_pendingHostFunction.kind != WasmSequencer_HostFunction::COMMAND) {
        this->sequencer_sendSignal_stmtResponse_unexpected();
    } else {
        this->m_pendingHostFunction.clear();

        // Respond to the host command by pushing a value to the Wasm operand stack
        spacewasm_value_t return_val;
        return_val.tag = SPACEWASM_I32;
        return_val.u.i32_ = static_cast<I32>(response.e);

        const auto status = spacewasm_resume_value(this->m_wasm, return_val);
        FW_ASSERT(status == SPACEWASM_OK, status);

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
    if (this->sequencer_getState() != WasmSequencer_SequencerStateMachine_State::IDLE &&
        this->sequencer_getState() != WasmSequencer_SequencerStateMachine_State::READY) {
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
    switch (this->sequencer_getState()) {
        case WasmSequencer_SequencerStateMachine_State::IDLE:
        case WasmSequencer_SequencerStateMachine_State::READY:
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
    // Loading is only valid from IDLE.
    if (this->sequencer_getState() != WasmSequencer_SequencerStateMachine_State::IDLE &&
        this->sequencer_getState() != WasmSequencer_SequencerStateMachine_State::READY) {
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
    if (this->sequencer_getState() != Svc_WasmSequencer_SequencerStateMachine::State::READY) {
        this->log_WARNING_LO_InvalidCommand(this->sequencer_getState());
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
    this->sequencer_sendSignal_cmd_CANCEL();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::PAUSE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    this->sequencer_sendSignal_cmd_PAUSE();
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
}

void WasmSequencer ::CONTINUE_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    switch (this->sequencer_getState()) {
        case WasmSequencer_SequencerStateMachine_State::RUNNING_AWAITING_RESPONSE:
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
    this->m_invokeStatus = spacewasm_find_export_func(this->m_wasm, value, "main", &funcIndex);

    if (this->m_invokeStatus == SPACEWASM_OK) {
        // Attempt to invoke the main function
        this->m_invokeStatus = spacewasm_invoke(this->m_wasm, value, funcIndex, nullptr, 0);
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
    this->log_WARNING_HI_ModuleLoadFailed(static_cast<WasmSequencer_Status::T>(this->m_loadStatus));
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_reportInvokeFailure(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->log_WARNING_HI_ModuleInvokeFailed(static_cast<WasmSequencer_Status::T>(this->m_invokeStatus));
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

    Os::File file;
    const Os::File::Status openStatus = file.open(filePath.toChar(), Os::File::OPEN_READ);
    if (openStatus != Os::File::Status::OP_OK) {
        this->log_WARNING_HI_FileOpenError(filePath, openStatus);
        this->m_loadStatus = SPACEWASM_ERR_READER_ERROR;
        return;
    }

    // Expose the open file to the streaming read trampoline for the duration of
    // this decode.
    this->m_loadFile = &file;

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

    this->m_loadStatus = spacewasm_load_module(
        this->m_wasm, moduleName.toChar(),
        [](void* userdata, const U8** outBuf, std::size_t* outLen) -> spacewasm_read_result_t {
            FW_ASSERT(userdata != nullptr);
            return static_cast<WasmSequencer*>(userdata)->readModuleChunk(outBuf, outLen);
        },
        this, alloc, &this->m_moduleIndex);

    spacewasm_allocator_destroy(alloc);
    this->m_loadFile = nullptr;
    file.close();
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_spin(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    FW_ASSERT(this->m_wasm);

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

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkStatementTimeout(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // TODO
}

void WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_action_checkShouldWake(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    // Check if we have overrun the timer
    FW_ASSERT(this->m_hasPendingTimer);

    const Fw::Time now = this->getTime();
    switch (now.compare(now, this->m_pendingTimer)) {
        case Fw::TimeComparison::LT:
            // No timer overrun
            break;
        case Fw::TimeComparison::EQ:
        case Fw::TimeComparison::GT: {
            // Timeout!
            const auto status = spacewasm_resume(this->m_wasm);
            FW_ASSERT(status == SPACEWASM_OK, status);

            this->sequencer_sendSignal_stmtResponse_success();
            break;
        }
        case Fw::TimeComparison::INCOMPARABLE:
            // Time base / context changed since we set the timer
            this->sequencer_sendSignal_result_timeOpFailed();
            break;
    }
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
    switch (this->m_pendingHostFunction.kind) {
        case WasmSequencer_HostFunction::NONE:
            // Invalid host function
            FW_ASSERT(false);
        case WasmSequencer_HostFunction::COMMAND: {
            Fw::ComBuffer cmd;

            // Write the CMD descriptor to the ComBuffer
            auto serStatus =
                cmd.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND));
            FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

            // Copy the com buffer from the guest memory into our memory
            auto status = spacewasm_mem_read(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr1,
                                             cmd.getBuffAddr() + sizeof(FwPacketDescriptorType),
                                             this->m_pendingHostFunction.len1);

            if (status != SPACEWASM_OK) {
                // TODO(tumbar) Validate pointer region synchronously and assert here
                this->log_WARNING_HI_HostFunctionInvalidPointer(Svc::WasmSequencer_HostFunction::COMMAND,
                                                                static_cast<WasmSequencer_Status::T>(status));

                this->sequencer_sendSignal_stmtResponse_failure();
            } else {
                // Memory read succeeded, update the ComBuffer to hold the encoded command
                serStatus = cmd.moveSerToOffset(this->m_pendingHostFunction.len1 + sizeof(FwPacketDescriptorType));
                FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

                // Dispatch command to CmdDisp
                // TODO(tumbar) Use ctx to keep track of which command was sent?
                this->cmdOut_out(0, cmd, 0);
            }

            break;
        }
        case WasmSequencer_HostFunction::TELEMETRY: {
            Fw::Time time;
            Fw::TlmBuffer tlmBuffer;

            auto valid = this->getTlmChan_out(0, this->m_pendingHostFunction.chan_id, time, tlmBuffer);

            // Write the response into guest memory
            FW_ASSERT(this->m_pendingHostFunction.len1 == Fw::Time::SERIALIZED_SIZE,
                      static_cast<FwAssertArgType>(this->m_pendingHostFunction.len1), Fw::Time::SERIALIZED_SIZE);
            Fw::LinearBufferTemplate<Fw::Time::SERIALIZED_SIZE> timeBuf;

            auto serStatus = time.serializeTo(timeBuf);
            FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

            // Write the time
            spacewasm_status_t status;
            status = spacewasm_mem_write(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr1,
                                         timeBuf.getBuffAddr(), this->m_pendingHostFunction.len1);
            if (status != SPACEWASM_OK) {
                // TODO(tumbar) Validate pointer region synchronously and assert here
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::TELEMETRY,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->sequencer_sendSignal_stmtResponse_failure();
                break;
            }

            // Write the value
            status = spacewasm_mem_write(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr2,
                                         tlmBuffer.getBuffAddr(), this->m_pendingHostFunction.len2);
            if (status != SPACEWASM_OK) {
                // TODO(tumbar) Validate pointer region synchronously and assert here
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::TELEMETRY,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->sequencer_sendSignal_stmtResponse_failure();
                break;
            }

            // Resume the interpreter
            spacewasm_value_t return_val;
            return_val.tag = SPACEWASM_I32;
            return_val.u.i32_ = static_cast<I32>(valid.e);
            status = spacewasm_resume_value(this->m_wasm, return_val);
            FW_ASSERT(status == SPACEWASM_OK, status);

            // Wake up the state machine
            this->sequencer_sendSignal_stmtResponse_success();
            break;
        }
        case WasmSequencer_HostFunction::PARAMETER: {
            Fw::ParamBuffer prmBuf;
            auto prmStatus = this->getParam_out(0, this->m_pendingHostFunction.prm_id, prmBuf);

            // Write the parameter to linear memory
            spacewasm_status_t status =
                spacewasm_mem_write(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr1,
                                    prmBuf.getBuffAddr(), this->m_pendingHostFunction.len1);
            if (status != SPACEWASM_OK) {
                // TODO(tumbar) Validate pointer region synchronously and assert here
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::PARAMETER,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->sequencer_sendSignal_stmtResponse_failure();
                break;
            }

            // Resume the interpreter
            spacewasm_value_t return_val;
            return_val.tag = SPACEWASM_I32;
            return_val.u.i32_ = static_cast<I32>(prmStatus.e);
            status = spacewasm_resume_value(this->m_wasm, return_val);
            FW_ASSERT(status == SPACEWASM_OK, status);

            // Wake up the state machine
            this->sequencer_sendSignal_stmtResponse_success();
            break;
        }
        case WasmSequencer_HostFunction::EVENT: {
            U8 stringStorage[FW_LOG_STRING_MAX_SIZE + 1];
            FW_ASSERT(this->m_pendingHostFunction.len1 <= FW_LOG_STRING_MAX_SIZE,
                      static_cast<FwAssertArgType>(this->m_pendingHostFunction.len1), FW_LOG_STRING_MAX_SIZE);
            const Fw::ExternalString msg(reinterpret_cast<char*>(stringStorage), FW_LOG_STRING_MAX_SIZE + 1);

            auto status = spacewasm_mem_read(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr1,
                                             stringStorage, this->m_pendingHostFunction.len1);
            stringStorage[this->m_pendingHostFunction.len1] = 0;
            if (status != SPACEWASM_OK) {
                // TODO(tumbar) Validate pointer region synchronously and assert here
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::EVENT,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->sequencer_sendSignal_stmtResponse_failure();
            } else {
                // Emit the event
                switch (this->m_pendingHostFunction.severity) {
                    case Fw::LogSeverity::FATAL:
                        this->log_FATAL_LogFatal(msg);
                        break;
                    case Fw::LogSeverity::WARNING_HI:
                        this->log_WARNING_HI_LogWarningHi(msg);
                        break;
                    case Fw::LogSeverity::WARNING_LO:
                        this->log_WARNING_LO_LogWarningLo(msg);
                        break;
                    case Fw::LogSeverity::COMMAND:
                        this->log_COMMAND_LogCommand(msg);
                        break;
                    case Fw::LogSeverity::ACTIVITY_HI:
                        this->log_ACTIVITY_HI_LogActivityHi(msg);
                        break;
                    case Fw::LogSeverity::ACTIVITY_LO:
                        this->log_ACTIVITY_LO_LogActivityLo(msg);
                        break;
                    case Fw::LogSeverity::DIAGNOSTIC:
                        this->log_DIAGNOSTIC_LogDiagnostic(msg);
                        break;
                }

                status = spacewasm_resume(this->m_wasm);
                FW_ASSERT(status == SPACEWASM_OK, status);

                this->sequencer_sendSignal_stmtResponse_success();
            }

            break;
        }
        case WasmSequencer_HostFunction::RSLEEP: {
            U32 seconds = static_cast<U32>(this->m_pendingHostFunction.time_us / 1000000);
            U32 useconds = static_cast<U32>(this->m_pendingHostFunction.time_us % 1000000);

            // Relative sleep from now
            Fw::Time timer = this->getTime();
            timer.add(seconds, useconds);

            this->m_pendingTimer = timer;
            this->m_hasPendingTimer = true;
            break;
        }
        case WasmSequencer_HostFunction::ASLEEP: {
            U32 seconds = static_cast<U32>(this->m_pendingHostFunction.time_us / 1000000);
            U32 useconds = static_cast<U32>(this->m_pendingHostFunction.time_us % 1000000);

            // Absolute is relative to epoch, we still need to get the time for base/context
            Fw::Time timer = this->getTime();
            timer.set(seconds, useconds);

            this->m_pendingTimer = timer;
            this->m_hasPendingTimer = true;
            break;
        }
    }
}
void WasmSequencer::Svc_WasmSequencer_SequencerStateMachine_action_clearPendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) {
    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::NONE;
    this->m_pendingHostFunction.caller = nullptr;
    this->m_hasPendingTimer = false;
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------
bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingRun(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_pendingRun;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_invokeFailed(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_invokeStatus != SPACEWASM_OK;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingPause(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_pendingPause;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_pendingHostFunction.isPending();
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_pendingTimer(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_hasPendingTimer;
}

bool WasmSequencer ::Svc_WasmSequencer_SequencerStateMachine_guard_moduleLoadSucceeded(
    SmId smId,
    Svc_WasmSequencer_SequencerStateMachine::Signal signal) const {
    return this->m_loadStatus == SPACEWASM_OK;
}

}  // namespace Svc
