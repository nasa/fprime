// ======================================================================
// \title  WasmSequencerEngine.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer engine state machine
// ======================================================================

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/LinearBufferTemplate.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Fw/Types/SuccessEnumAc.hpp"
#include "Svc/Seq/BlockStateEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_TrapReasonEnumAc.hpp"
#include "config/FwAssertArgTypeAliasAc.h"
#include "spacewasm.h"

namespace Svc {

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_signalEntered(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->interpreter_sendSignal_entered();
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_spin(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    FW_ASSERT(this->m_wasm);

    Fw::ParamValid prmValid;
    const auto fuel = this->paramGet_INSTRUCTION_FUEL(prmValid);

    spacewasm_trap_t trap = SPACEWASM_TRAP_NONE;
    const spacewasm_run_status_t runStatus = spacewasm_run(this->m_wasm, fuel, &trap);

    switch (runStatus) {
        case SPACEWASM_RUN_FINISHED: {
            spacewasm_value_t result;
            auto status = spacewasm_get_result(this->m_wasm, spacewasm_valtype_t::SPACEWASM_I32, &result);
            if (status == SPACEWASM_ERR_NOT_FOUND) {
                // Return status code was "void" (success)
                this->interpreter_sendSignal_interpreterFinished(0);
            } else {
                FW_ASSERT(status == SPACEWASM_OK);
                FW_ASSERT(result.tag == spacewasm_valtype_t::SPACEWASM_I32);
                this->interpreter_sendSignal_interpreterFinished(result.u.i32_);
            }

            break;
        }
        case SPACEWASM_RUN_TRAP:
            // Filter out HOST traps due to exit/panic. These host functions just use trap as a mechanism
            // to kill the interpreter.
            if (trap == SPACEWASM_TRAP_HOST && (this->m_exit.reason == WasmSequencer_ExitReason::HOST_EXIT ||
                                                this->m_exit.reason == WasmSequencer_ExitReason::HOST_PANIC)) {
                this->interpreter_sendSignal_interpreterTrap(WasmSequencer_TrapReason::NONE);
            } else {
                this->interpreter_sendSignal_interpreterTrap(WasmSequencer::mapTrapReason(trap));
            }
            break;
        case SPACEWASM_RUN_PAUSE:
            this->interpreter_sendSignal_interpreterPause();
            break;
        case SPACEWASM_RUN_OUT_OF_FUEL:
            this->interpreter_sendSignal_interpreterOutOfFuel();
            break;
        default:
            FW_ASSERT(false, runStatus);
            break;
    }
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_reset(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    FW_ASSERT(this->m_wasm);
    auto status = spacewasm_reset(this->m_wasm);
    FW_ASSERT(status == SPACEWASM_OK);

    // This only resets the spacewasm engine's execution state (operand stack, pc,
    // fp, sp). The exit-status record (reason / last host function / trap reason) is
    // cleared separately by clearExitStatus.
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_clearExitStatus(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_exit.reason = WasmSequencer_ExitReason::UNKNOWN;
    this->m_exit.lastHostFunction = WasmSequencer_HostFunction::NONE;
    this->m_exit.code = 0;
    this->m_exit.lastTrapReason = WasmSequencer_TrapReason::NONE;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_INTERPRETER_FINISHED(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_exit.reason = WasmSequencer_ExitReason::INTERPRETER_FINISHED;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_INTERPRETER_TRAP(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    if (this->m_exit.reason == WasmSequencer_ExitReason::UNKNOWN) {
        this->m_exit.reason = WasmSequencer_ExitReason::INTERPRETER_TRAP;
    }
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_REPLY_TIMEOUT(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_exit.reason = WasmSequencer_ExitReason::REPLY_TIMEOUT;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_HOST_FAILURE(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_exit.reason = WasmSequencer_ExitReason::HOST_FAILURE;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_UNEXPECTED_REPLY(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_exit.reason = WasmSequencer_ExitReason::UNEXPECTED_REPLY;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_TIMER_INCOMPARABLE(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_exit.reason = WasmSequencer_ExitReason::TIMER_INCOMPARABLE;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setExitReason_CANCEL(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_exit.reason = WasmSequencer_ExitReason::CANCEL;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setExitCode(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal,
    I32 value) {
    this->m_exit.code = value;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setTrapReason(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal,
    const Svc::WasmSequencer_TrapReason& value) {
    this->m_exit.lastTrapReason = value;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setLastHostFunction(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_exit.lastHostFunction = this->m_pendingHostFunction.kind;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_finish(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->controller_sendSignal_engineFinished(this->m_executingContext);
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_reportPaused(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->log_ACTIVITY_HI_SequencePaused();
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_clearPause(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_pendingPause = false;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_dispatchPendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    switch (this->m_pendingHostFunction.kind) {
        case WasmSequencer_HostFunction::NONE:
            // Invalid host function
            FW_ASSERT(false);
            break;
        case WasmSequencer_HostFunction::COMMAND:
            this->dispatchCommand();
            break;
        case WasmSequencer_HostFunction::TELEMETRY:
            this->dispatchTelemetry();
            break;
        case WasmSequencer_HostFunction::PARAMETER:
            this->dispatchParameter();
            break;
        case WasmSequencer_HostFunction::EVENT:
            this->dispatchEvent();
            break;
        case WasmSequencer_HostFunction::RSLEEP:
            this->dispatchRelativeSleep();
            break;
        case WasmSequencer_HostFunction::ASLEEP:
            this->dispatchAbsoluteSleep();
            break;
        case WasmSequencer_HostFunction::ARGS:
            this->dispatchArgs();
            break;
        case WasmSequencer_HostFunction::TIME:
            this->dispatchTime();
            break;
        case WasmSequencer_HostFunction::SERIAL_OUT:
            this->dispatchSerialOut();
            break;
        case WasmSequencer_HostFunction::SERIAL_RECV:
            this->dispatchSerialRecv();
            break;
    }
}

Fw::Success WasmSequencer ::readGuestOrFail(WasmSequencer_HostFunction::T kind, U32 addr, U8* dst, FwSizeType len) {
    const spacewasm_status_t status = spacewasm_mem_read(this->m_pendingHostFunction.caller, addr, dst, len);
    if (status != SPACEWASM_OK) {
        this->log_WARNING_HI_HostFunctionInvalidPointer(kind, static_cast<WasmSequencer_Status::T>(status));
        this->interpreter_sendSignal_hostResponseFailure();
        return Fw::Success::FAILURE;
    } else {
        return Fw::Success::SUCCESS;
    }
}

Fw::Success WasmSequencer ::writeGuestOrFail(WasmSequencer_HostFunction::T kind,
                                             U32 addr,
                                             const U8* src,
                                             FwSizeType len) {
    const spacewasm_status_t status = spacewasm_mem_write(this->m_pendingHostFunction.caller, addr, src, len);
    if (status != SPACEWASM_OK) {
        this->log_WARNING_HI_HostFunctionInvalidPointer(kind, static_cast<WasmSequencer_Status::T>(status));
        return Fw::Success::FAILURE;
    } else {
        return Fw::Success::SUCCESS;
    }
}

// ----------------------------------------------------------------------
// Per-host-function dispatch helpers (arms of dispatchPendingHostFunction)
// ----------------------------------------------------------------------

void WasmSequencer ::dispatchCommand() {
    Fw::ComBuffer cmd;

    // Write the CMD descriptor to the ComBuffer
    auto serStatus = cmd.serializeFrom(static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND));
    FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

    // Copy the com buffer from the guest memory into our memory
    if (this->readGuestOrFail(Svc::WasmSequencer_HostFunction::COMMAND, this->m_pendingHostFunction.u.command.ptr,
                              cmd.getBuffAddr() + sizeof(FwPacketDescriptorType),
                              this->m_pendingHostFunction.u.command.len) != Fw::Success::SUCCESS) {
        return;
    }

    // Memory read succeeded, update the ComBuffer to hold the encoded command
    serStatus = cmd.moveSerToOffset(this->m_pendingHostFunction.u.command.len + sizeof(FwPacketDescriptorType));
    FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

    // Dispatch command to CmdDisp. The command context (cmdUid)
    // encodes the current sequence + command instance so we can
    // reject late/stale responses in cmdResponseIn_handler.
    this->m_tlm.commandsDispatched++;

    // Start the host-function timeout clock: we are about to block in
    // AWAITING_RESPONSE until the command response comes back in.
    this->m_hostFunctionStart = this->getTime();
    this->m_hasHostFunctionStart = true;

    this->cmdOut_out(0, cmd, this->makeCmdUid());
}

void WasmSequencer ::dispatchTelemetry() {
    Fw::Time time;
    Fw::TlmBuffer tlmBuffer;

    auto valid = this->getTlmChan_out(0, this->m_pendingHostFunction.u.telemetry.chanId, time, tlmBuffer);

    // Write the response into guest memory
    FW_ASSERT(this->m_pendingHostFunction.u.telemetry.timeLen == Fw::Time::SERIALIZED_SIZE,
              static_cast<FwAssertArgType>(this->m_pendingHostFunction.u.telemetry.timeLen), Fw::Time::SERIALIZED_SIZE);
    Fw::LinearBufferTemplate<Fw::Time::SERIALIZED_SIZE> timeBuf;

    auto serStatus = time.serializeTo(timeBuf);
    FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

    // Write the time
    if (this->writeGuestOrFail(Svc::WasmSequencer_HostFunction::TELEMETRY,
                               this->m_pendingHostFunction.u.telemetry.timePtr, timeBuf.getBuffAddr(),
                               this->m_pendingHostFunction.u.telemetry.timeLen) != Fw::Success::SUCCESS) {
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    if (tlmBuffer.getSize() > this->m_pendingHostFunction.u.telemetry.valueLen) {
        this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::TELEMETRY,
                                            this->m_pendingHostFunction.u.telemetry.valueLen,
                                            static_cast<U32>(tlmBuffer.getSize()));
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    // Write the value
    if (this->writeGuestOrFail(Svc::WasmSequencer_HostFunction::TELEMETRY,
                               this->m_pendingHostFunction.u.telemetry.valuePtr, tlmBuffer.getBuffAddr(),
                               tlmBuffer.getSize()) != Fw::Success::SUCCESS) {
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(valid.e));
}

void WasmSequencer ::dispatchParameter() {
    Fw::ParamBuffer prmBuf;
    auto prmStatus = this->getParam_out(0, this->m_pendingHostFunction.u.parameter.prmId, prmBuf);

    if (prmBuf.getSize() > this->m_pendingHostFunction.u.parameter.len) {
        this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::PARAMETER,
                                            this->m_pendingHostFunction.u.parameter.len,
                                            static_cast<U32>(prmBuf.getSize()));
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    // Write the parameter to linear memory
    if (this->writeGuestOrFail(Svc::WasmSequencer_HostFunction::PARAMETER, this->m_pendingHostFunction.u.parameter.ptr,
                               prmBuf.getBuffAddr(), prmBuf.getSize()) != Fw::Success::SUCCESS) {
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(prmStatus.e));
}

void WasmSequencer ::dispatchEvent() {
    U8 stringStorage[FW_LOG_STRING_MAX_SIZE + 1];
    FW_ASSERT(this->m_pendingHostFunction.u.event.msgLen <= FW_LOG_STRING_MAX_SIZE,
              static_cast<FwAssertArgType>(this->m_pendingHostFunction.u.event.msgLen), FW_LOG_STRING_MAX_SIZE);
    const Fw::ExternalString msg(reinterpret_cast<char*>(stringStorage), FW_LOG_STRING_MAX_SIZE + 1);

    if (this->readGuestOrFail(Svc::WasmSequencer_HostFunction::EVENT, this->m_pendingHostFunction.u.event.msgPtr,
                              stringStorage, this->m_pendingHostFunction.u.event.msgLen) != Fw::Success::SUCCESS) {
        return;
    }
    stringStorage[this->m_pendingHostFunction.u.event.msgLen] = 0;

    // Emit the event at the guest-requested severity. FATAL and
    // COMMAND are forbidden for guest programs (FATAL would let
    // untrusted code trigger the FatalHandler; COMMAND is reserved
    // for the command dispatcher). A forbidden or out-of-range
    // severity is reported via HostFunctionInvalidSeverity, carrying
    // the raw id and the guest message, and the guest continues.
    const I32 rawSeverity = static_cast<I32>(this->m_pendingHostFunction.u.event.rawSeverity);
    switch (static_cast<Fw::LogSeverity::T>(rawSeverity)) {
        case Fw::LogSeverity::WARNING_HI:
            this->log_WARNING_HI_LogWarningHi(msg);
            break;
        case Fw::LogSeverity::WARNING_LO:
            this->log_WARNING_LO_LogWarningLo(msg);
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
        case Fw::LogSeverity::FATAL:
        case Fw::LogSeverity::COMMAND:
        default:
            this->log_WARNING_HI_HostFunctionInvalidSeverity(rawSeverity, msg);
            break;
    }

    this->interpreter_sendSignal_hostResume();
}

void WasmSequencer ::dispatchRelativeSleep() {
    const U32 seconds = static_cast<U32>(this->m_pendingHostFunction.u.rsleep.us / 1000000);
    const U32 useconds = static_cast<U32>(this->m_pendingHostFunction.u.rsleep.us % 1000000);

    const Fw::Time now = this->getTime();
    const U64 deadlineSeconds =
        static_cast<U64>(now.getSeconds()) + seconds + (static_cast<U64>(now.getUSeconds()) + useconds) / 1000000u;

    Fw::Time timer = now;
    if (deadlineSeconds > static_cast<U64>(std::numeric_limits<U32>::max())) {
        timer.set(std::numeric_limits<U32>::max(), 999999u);
    } else {
        timer.add(seconds, useconds);
    }

    this->m_pendingTimer = timer;
    this->m_hasPendingTimer = true;
}

void WasmSequencer ::dispatchAbsoluteSleep() {
    U32 seconds = static_cast<U32>(this->m_pendingHostFunction.u.asleep.us / 1000000);
    U32 useconds = static_cast<U32>(this->m_pendingHostFunction.u.asleep.us % 1000000);

    // Absolute is relative to epoch, we still need to get the time for base/context
    Fw::Time timer = this->getTime();
    timer.set(seconds, useconds);

    this->m_pendingTimer = timer;
    this->m_hasPendingTimer = true;
}

void WasmSequencer ::dispatchArgs() {
    const FwSizeType argCapacity = static_cast<FwSizeType>(sizeof(this->m_args.get_buffer()));
    if (this->m_args.get_size() > argCapacity) {
        this->log_WARNING_HI_BufferTooLarge(WasmSequencer_HostFunction::ARGS, static_cast<U32>(this->m_args.get_size()),
                                            static_cast<U32>(argCapacity));
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    if (this->m_args.get_size() > this->m_pendingHostFunction.u.args.len) {
        // Too many param bytes and we are going to leak data into the guest memory
        this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::ARGS, this->m_pendingHostFunction.u.args.len,
                                            static_cast<U32>(this->m_args.get_size()));
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    // Write the arguments to linear memory
    if (this->writeGuestOrFail(Svc::WasmSequencer_HostFunction::ARGS, this->m_pendingHostFunction.u.args.ptr,
                               this->m_args.get_buffer(), this->m_args.get_size()) != Fw::Success::SUCCESS) {
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(this->m_args.get_size()));
}

void WasmSequencer ::dispatchTime() {
    auto time = this->getTime();

    FW_ASSERT(this->m_pendingHostFunction.u.time.len == Fw::Time::SERIALIZED_SIZE,
              static_cast<FwAssertArgType>(this->m_pendingHostFunction.u.time.len), Fw::Time::SERIALIZED_SIZE);
    Fw::LinearBufferTemplate<Fw::Time::SERIALIZED_SIZE> timeBuf;

    auto serStatus = time.serializeTo(timeBuf);
    FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

    // Write the time
    if (this->writeGuestOrFail(Svc::WasmSequencer_HostFunction::TIME, this->m_pendingHostFunction.u.time.ptr,
                               timeBuf.getBuffAddr(), this->m_pendingHostFunction.u.time.len) != Fw::Success::SUCCESS) {
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    this->interpreter_sendSignal_hostResume();
}

void WasmSequencer ::dispatchSerialOut() {
    const FwIndexType portNum = static_cast<FwIndexType>(this->m_pendingHostFunction.u.serialOut.index);

    // Copy the payload out of guest memory into our own buffer
    if (this->readGuestOrFail(Svc::WasmSequencer_HostFunction::SERIAL_OUT, this->m_pendingHostFunction.u.serialOut.ptr,
                              this->m_serialOutBuffer.getBuffAddr(),
                              this->m_pendingHostFunction.u.serialOut.len) != Fw::Success::SUCCESS) {
        return;
    }

    auto serStatus = this->m_serialOutBuffer.setBuffLen(this->m_pendingHostFunction.u.serialOut.len);
    FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

    // Invoke the serial output port.
    serStatus = this->serialOut_out(portNum, this->m_serialOutBuffer);
    if (serStatus != Fw::FW_SERIALIZE_OK) {
        this->log_WARNING_HI_SerialPortSendFailed(Svc::WasmSequencer_HostFunction::SERIAL_OUT,
                                                  static_cast<I32>(serStatus));
        this->interpreter_sendSignal_hostResponseFailure();
        return;
    }

    // Send worked, wake up the interpreter
    this->interpreter_sendSignal_hostResume();
}

void WasmSequencer ::dispatchSerialRecv() {
    const FwIndexType portNum = static_cast<FwIndexType>(this->m_pendingHostFunction.u.serialRecv.index);
    FW_ASSERT(portNum < NUM_SERIALIN_INPUT_PORTS, portNum);
    auto& queue = this->m_serialInQueue[portNum];

    // Check if there is an available message on the queue
    if (queue.get_allocated_size() > 0) {
        // There is data available signal to the state machine to pull this data and wake up without blocking
        this->interpreter_sendSignal_serialInMessage(portNum);
    } else {
        // Check if we should block or not
        switch (this->m_pendingHostFunction.u.serialRecv.blockingType) {
            case Svc::BlockState::BLOCK:
                // Do not immediately resume the interpreter, this will let the state machine asynchronously
                // wait for a signal that we got a message (or timeout).
                this->m_hostFunctionStart = this->getTime();
                this->m_hasHostFunctionStart = true;
                break;
            case Svc::BlockState::NO_BLOCK:
                // We are non-blocking and the queue is empty, report this back to the interpreter and wake back
                // up
                constexpr I32 FPRIME_SERIAL_RECV_QUEUE_STATUS_EMPTY = 1;
                this->interpreter_sendSignal_hostResumeI32(FPRIME_SERIAL_RECV_QUEUE_STATUS_EMPTY);
                break;
        }
    }
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_clearPendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::NONE;
    this->m_pendingHostFunction.caller = nullptr;
    this->m_hasPendingTimer = false;
    this->m_hasHostFunctionStart = false;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_setContext(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal,
    const Svc::WasmSequencer_RequestContext& value) {
    FW_ASSERT(!this->m_hasExecutingContext);
    this->m_hasExecutingContext = true;
    this->m_executingContext = value;

    // A fresh execution window is starting. Bump the sequence counter so any command
    // dispatched from this program carries a distinct cmdUid (see makeCmdUid), letting
    // cmdResponseIn_handler recognize a response that arrives late, after its
    // originating sequence has ended.
    this->m_sequencesStarted++;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_clearContext(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    this->m_hasExecutingContext = false;
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_resume(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    FW_ASSERT(this->m_wasm != nullptr);
    auto status = spacewasm_resume(this->m_wasm);
    FW_ASSERT(status == SPACEWASM_OK, status);
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_resumeI32(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal,
    I32 value) {
    FW_ASSERT(this->m_wasm != nullptr);
    spacewasm_value_t return_val;
    return_val.tag = SPACEWASM_I32;
    return_val.u.i32_ = value;

    auto status = spacewasm_resume_value(this->m_wasm, return_val);
    FW_ASSERT(status == SPACEWASM_OK, status);
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_checkSleepTimers(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    // Check if we have overrun the timer
    FW_ASSERT(this->m_hasPendingTimer);

    const Fw::Time now = this->getTime();
    switch (Fw::Time::compare(now, this->m_pendingTimer)) {
        case Fw::TimeComparison::LT:
            // No timer overrun
            break;
        case Fw::TimeComparison::EQ:
        case Fw::TimeComparison::GT: {
            // Timeout!
            this->interpreter_sendSignal_hostResume();
            break;
        }
        case Fw::TimeComparison::INCOMPARABLE:
            // Time base / context changed since we set the timer
            this->interpreter_sendSignal_hostResponseTimeIncomparable();
            break;
    }
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_checkTimeout(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) {
    // Only blocking host functions that await an external event are subject to the
    // host-function timeout (COMMAND -> cmdResponseIn, blocking SERIAL_RECV -> serialIn).
    // Sleeps have their own wake timer (checkSleepTimers), separate from this timeout.
    if (!this->m_hasHostFunctionStart) {
        return;
    }

    Fw::ParamValid prmValid;
    const F32 timeoutSecs = this->paramGet_HOST_FUNCTION_TIMEOUT_SECS(prmValid);

    // A non-positive or out-of-range timeout disables the check entirely.
    if (timeoutSecs <= 0.0f || timeoutSecs > static_cast<F32>(std::numeric_limits<U32>::max())) {
        return;
    }

    // Deadline = function start + timeout. Round microseconds up so the timeout
    // is never reported early.
    U32 seconds = static_cast<U32>(timeoutSecs);
    U32 useconds = static_cast<U32>((timeoutSecs - static_cast<F32>(seconds)) * 1000000.0f + 0.5f);

    // Rounding up can push the microsecond field to 1000000; carry it into
    // seconds so Fw::Time::add() always receives a normalized (< 1e6) value.
    if (useconds >= 1000000u) {
        seconds += useconds / 1000000u;
        useconds %= 1000000u;
    }

    Fw::Time deadline = this->m_hostFunctionStart;
    deadline.add(seconds, useconds);

    const Fw::Time now = this->getTime();
    switch (Fw::Time::compare(now, deadline)) {
        case Fw::TimeComparison::LT:
            // Deadline not yet reached.
            break;
        case Fw::TimeComparison::EQ:
        case Fw::TimeComparison::GT:
            // Host function timed out waiting for its reply.
            this->interpreter_sendSignal_hostResponseTimeout();
            break;
        case Fw::TimeComparison::INCOMPARABLE:
            // Time base / context changed since the host function started.
            this->interpreter_sendSignal_hostResponseTimeIncomparable();
            break;
    }
}

void WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_action_dequeueSerialAndResume(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal,
    const FwIndexType& value) {
    const FwIndexType portNum = static_cast<FwIndexType>(this->m_pendingHostFunction.u.serialRecv.index);
    FW_ASSERT(portNum < NUM_SERIALIN_INPUT_PORTS, portNum);
    auto& queue = this->m_serialInQueue[portNum];

    this->m_dequeueSucceeded = false;

    // Message is available, pull out the size
    U32 msgSize;
    auto status = queue.peek(msgSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    if (msgSize > this->m_pendingHostFunction.u.serialRecv.dataSize) {
        this->log_WARNING_HI_BufferTooSmall(Svc::WasmSequencer_HostFunction::SERIAL_RECV,
                                            this->m_pendingHostFunction.u.serialRecv.dataSize, msgSize);
        return;
    }

    // The queue holds [U32 size][payload]; skip the size prefix when copying the payload.
    const U32 queuePayloadStart = sizeof(U32);
    const U32 dataSize = static_cast<U32>(sizeof(U32)) + msgSize;

    // Write the message size in little endian to the guest memory
    Fw::LinearBufferTemplate<sizeof(U32)> msgSizeSer;
    status = msgSizeSer.serializeFrom(msgSize, Fw::Endianness::LITTLE);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

    if (this->writeGuestOrFail(Svc::WasmSequencer_HostFunction::SERIAL_RECV,
                               this->m_pendingHostFunction.u.serialRecv.actualSizePtr, msgSizeSer.getBuffAddr(),
                               sizeof(U32)) != Fw::Success::SUCCESS) {
        return;
    }

    // Pull the message off the queue, we may need to do it in multiple chunks
    constexpr U32 CHUNK_SIZE = 32;
    U8 scratch[CHUNK_SIZE];

    // Pull all the full chunks off the queue
    U32 queueOffset = queuePayloadStart;
    for (; (queueOffset + CHUNK_SIZE) <= dataSize; queueOffset += CHUNK_SIZE) {
        status = queue.peek(scratch, CHUNK_SIZE, queueOffset);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

        // Copy the data into the guest memory
        if (this->writeGuestOrFail(Svc::WasmSequencer_HostFunction::SERIAL_RECV,
                                   this->m_pendingHostFunction.u.serialRecv.dataPtr + (queueOffset - queuePayloadStart),
                                   scratch, CHUNK_SIZE) != Fw::Success::SUCCESS) {
            return;
        }
    }

    // Pull out the final partial chunk
    if (queueOffset < dataSize) {
        const U32 remaining = dataSize - queueOffset;
        FW_ASSERT(remaining < CHUNK_SIZE, static_cast<FwAssertArgType>(dataSize),
                  static_cast<FwAssertArgType>(queueOffset), CHUNK_SIZE);

        status = queue.peek(scratch, remaining, queueOffset);
        FW_ASSERT(status == Fw::FW_SERIALIZE_OK, status);

        // Copy the data into the guest memory
        if (this->writeGuestOrFail(Svc::WasmSequencer_HostFunction::SERIAL_RECV,
                                   this->m_pendingHostFunction.u.serialRecv.dataPtr + (queueOffset - queuePayloadStart),
                                   scratch, remaining) != Fw::Success::SUCCESS) {
            return;
        }
    }

    // Dequeue the message now that we fully copied it into guest memory
    status = queue.rotate(dataSize);
    FW_ASSERT(status == Fw::FW_SERIALIZE_OK);

    this->m_dequeueSucceeded = true;

    // Yay, we successfully dequeued a message from the queue, wake up the interpreter to tell it about our success
    constexpr I32 FPRIME_SERIAL_RECV_QUEUE_STATUS_OK = 0;
    spacewasm_value_t return_val;
    return_val.tag = SPACEWASM_I32;
    return_val.u.i32_ = FPRIME_SERIAL_RECV_QUEUE_STATUS_OK;

    auto resumeStatus = spacewasm_resume_value(this->m_wasm, return_val);
    FW_ASSERT(resumeStatus == SPACEWASM_OK, resumeStatus);
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_guard_pendingPause(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) const {
    return this->m_pendingPause;
}

bool WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_guard_pendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) const {
    return this->m_pendingHostFunction.isPending();
}

bool WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_guard_pendingHostFunctionIsSleep(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) const {
    return this->m_pendingHostFunction.kind == WasmSequencer_HostFunction::ASLEEP ||
           this->m_pendingHostFunction.kind == WasmSequencer_HostFunction::RSLEEP;
}

bool WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_guard_blockingSerialIn(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal,
    const FwIndexType& value) const {
    return (this->m_pendingHostFunction.kind == Svc::WasmSequencer_HostFunction::SERIAL_RECV &&
            this->m_pendingHostFunction.u.serialRecv.index == static_cast<U32>(value));
}

bool WasmSequencer ::Svc_WasmSequencer_InterpreterStateMachine_guard_deqeueSucceeded(
    SmId smId,
    Svc_WasmSequencer_InterpreterStateMachine::Signal signal) const {
    return this->m_dequeueSucceeded;
}

}  // namespace Svc
