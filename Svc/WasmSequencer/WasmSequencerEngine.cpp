// ======================================================================
// \title  WasmSequencer.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer component implementation class
// ======================================================================

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/Assert.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_TrapReasonEnumAc.hpp"
#include "spacewasm.h"

namespace Svc {

// ----------------------------------------------------------------------
// Implementations for internal state machine actions
// ----------------------------------------------------------------------

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_signalEntered(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->interpreter_sendSignal_entered();
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_spin(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
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
            if (trap == SPACEWASM_TRAP_HOST && (this->m_exitReason == WasmSequencer_ExitReason::HOST_EXIT ||
                                                this->m_exitReason == WasmSequencer_ExitReason::HOST_PANIC)) {
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

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_reset(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    FW_ASSERT(this->m_wasm);
    auto status = spacewasm_reset(this->m_wasm);
    FW_ASSERT(status == SPACEWASM_OK);

    // Clear the failed-host-function record so a non-host failure (e.g. a
    // bytecode trap) in the next run reports NONE rather than a stale value.
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_clearExitStatus(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_exitReason = WasmSequencer_ExitReason::UNKNOWN;
    this->m_failedHostFunction = WasmSequencer_HostFunction::NONE;
    this->m_exitCode = 0;
    this->m_tlmLastTrapReason = WasmSequencer_TrapReason::NONE;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_setExitReason_INTERPRETER_FINISHED(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_exitReason = WasmSequencer_ExitReason::INTERPRETER_FINISHED;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_setExitReason_INTERPRETER_TRAP(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    if (this->m_exitReason == WasmSequencer_ExitReason::UNKNOWN) {
        this->m_exitReason = WasmSequencer_ExitReason::INTERPRETER_TRAP;
    }
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_setExitReason_REPLY_TIMEOUT(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_exitReason = WasmSequencer_ExitReason::REPLY_TIMEOUT;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_setExitReason_HOST_FAILURE(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_exitReason = WasmSequencer_ExitReason::HOST_FAILURE;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_setExitReason_UNEXPECTED_REPLY(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_exitReason = WasmSequencer_ExitReason::UNEXPECTED_REPLY;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_setExitReason_TIMER_INCOMPARABLE(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_exitReason = WasmSequencer_ExitReason::TIMER_INCOMPARABLE;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_setExitReason_CANCEL(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_exitReason = WasmSequencer_ExitReason::CANCEL;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_setExitCode(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal,
    I32 value) {
    this->m_exitCode = value;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_setTrapReason(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal,
    const Svc::WasmSequencer_TrapReason& value) {
    this->m_tlmLastTrapReason = value;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_updateHostFailureReason(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    if (this->m_pendingHostFunction.isPending()) {
        this->m_failedHostFunction = this->m_pendingHostFunction.kind;
    }
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_finish(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->controller_sendSignal_engineFinished();
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_reportPaused(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->log_ACTIVITY_HI_SequencePaused();
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_pendPause(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_pendingPause = true;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_clearPause(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_pendingPause = false;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_dispatchPendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    switch (this->m_pendingHostFunction.kind) {
        case WasmSequencer_HostFunction::NONE:
            // Invalid host function
            FW_ASSERT(false);
            break;
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
                this->log_WARNING_HI_HostFunctionInvalidPointer(Svc::WasmSequencer_HostFunction::COMMAND,
                                                                static_cast<WasmSequencer_Status::T>(status));

                this->interpreter_sendSignal_hostResponseFailure();
            } else {
                // Memory read succeeded, update the ComBuffer to hold the encoded command
                serStatus = cmd.moveSerToOffset(this->m_pendingHostFunction.len1 + sizeof(FwPacketDescriptorType));
                FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

                // Dispatch command to CmdDisp. The command context (cmdUid)
                // encodes the current sequence + command instance so we can
                // reject late/stale responses in cmdResponseIn_handler.
                this->m_tlmCommandsDispatched++;

                // Start the statement-timeout clock: we are about to block in
                // AWAITING_RESPONSE until the command response comes back in.
                this->m_statementStart = this->getTime();
                this->m_hasStatementStart = true;

                this->cmdOut_out(0, cmd, this->makeCmdUid());
            }

            break;
        }
        case WasmSequencer_HostFunction::TELEMETRY: {
            Fw::Time time;
            Fw::TlmBuffer tlmBuffer;

            auto valid =
                this->getTlmChan_out(0, static_cast<FwChanIdType>(this->m_pendingHostFunction.id), time, tlmBuffer);

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
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::TELEMETRY,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            if (tlmBuffer.getSize() > this->m_pendingHostFunction.len2) {
                this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::TELEMETRY,
                                                    this->m_pendingHostFunction.len2,
                                                    static_cast<U32>(tlmBuffer.getSize()));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            // Write the value
            status = spacewasm_mem_write(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr2,
                                         tlmBuffer.getBuffAddr(), tlmBuffer.getSize());
            if (status != SPACEWASM_OK) {
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::TELEMETRY,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(valid.e));
            break;
        }
        case WasmSequencer_HostFunction::PARAMETER: {
            Fw::ParamBuffer prmBuf;
            auto prmStatus = this->getParam_out(0, static_cast<FwPrmIdType>(this->m_pendingHostFunction.id), prmBuf);

            if (prmBuf.getSize() > this->m_pendingHostFunction.len1) {
                this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::PARAMETER,
                                                    this->m_pendingHostFunction.len1,
                                                    static_cast<U32>(prmBuf.getSize()));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            // Write the parameter to linear memory
            spacewasm_status_t status =
                spacewasm_mem_write(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr1,
                                    prmBuf.getBuffAddr(), prmBuf.getSize());
            if (status != SPACEWASM_OK) {
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::PARAMETER,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(prmStatus.e));
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
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::EVENT,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->interpreter_sendSignal_hostResponseFailure();
            } else {
                // Emit the event at the guest-requested severity. FATAL and
                // COMMAND are forbidden for guest programs (FATAL would let
                // untrusted code trigger the FatalHandler; COMMAND is reserved
                // for the command dispatcher). A forbidden or out-of-range
                // severity is reported via HostFunctionInvalidSeverity, carrying
                // the raw id and the guest message, and the guest continues.
                const I32 rawSeverity = static_cast<I32>(this->m_pendingHostFunction.id);
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
        case WasmSequencer_HostFunction::ARGS: {
            if (this->m_args.get_size() > this->m_pendingHostFunction.len1) {
                // Too many param bytes and we are going to leak data into the guest memory
                this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::ARGS, this->m_pendingHostFunction.len1,
                                                    static_cast<U32>(this->m_args.get_size()));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            // Write the parameter to linear memory
            spacewasm_status_t status =
                spacewasm_mem_write(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr1,
                                    this->m_args.get_buffer(), this->m_args.get_size());
            if (status != SPACEWASM_OK) {
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::ARGS,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            this->interpreter_sendSignal_hostResumeI32(static_cast<I32>(this->m_args.get_size()));
            break;
        }
        case WasmSequencer_HostFunction::TIME: {
            auto time = this->getTime();

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
                this->log_WARNING_HI_HostFunctionInvalidPointer(
                    Svc::WasmSequencer_HostFunction::TIME,
                    Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            this->interpreter_sendSignal_hostResume();
            break;
        }
        case WasmSequencer_HostFunction::SYNC_PORT: {
            const FwIndexType portNum = static_cast<FwIndexType>(this->m_pendingHostFunction.id);

            // Copy the payload out of guest memory into our own buffer
            auto status = spacewasm_mem_read(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr1,
                                             this->m_serialPortBuffer.getBuffAddr(), this->m_pendingHostFunction.len1);
            if (status != SPACEWASM_OK) {
                this->log_WARNING_HI_HostFunctionInvalidPointer(Svc::WasmSequencer_HostFunction::SYNC_PORT,
                                                                static_cast<WasmSequencer_Status::T>(status));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            auto serStatus = this->m_serialPortBuffer.setBuffLen(this->m_pendingHostFunction.len1);
            FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

            // Invoke the serial output port. The reply port MUST NOT be connected for the
            // synchronous variant, so we resume the interpreter immediately without waiting.
            serStatus = this->serialOut_out(portNum, this->m_serialPortBuffer);
            FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

            this->interpreter_sendSignal_hostResume();
            break;
        }
        case WasmSequencer_HostFunction::ASYNC_PORT: {
            const FwIndexType portNum = static_cast<FwIndexType>(this->m_pendingHostFunction.id);

            // Copy the payload out of guest memory into our own buffer
            auto status = spacewasm_mem_read(this->m_pendingHostFunction.caller, this->m_pendingHostFunction.ptr1,
                                             this->m_serialPortBuffer.getBuffAddr(), this->m_pendingHostFunction.len1);
            if (status != SPACEWASM_OK) {
                this->log_WARNING_HI_HostFunctionInvalidPointer(Svc::WasmSequencer_HostFunction::ASYNC_PORT,
                                                                static_cast<WasmSequencer_Status::T>(status));
                this->interpreter_sendSignal_hostResponseFailure();
                break;
            }

            auto serStatus = this->m_serialPortBuffer.setBuffLen(this->m_pendingHostFunction.len1);
            FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

            // Start the statement-timeout clock: we are about to block in
            // AWAITING_RESPONSE until the reply comes back on serialReply[portNum].
            this->m_statementStart = this->getTime();
            this->m_hasStatementStart = true;

            // Invoke the serial output port and block the interpreter until the reply
            // comes back on serialReply[portNum] (handled by serialReply_handler)
            serStatus = this->serialOut_out(portNum, this->m_serialPortBuffer);
            FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

            break;
        }
    }
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_clearPendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::NONE;
    this->m_pendingHostFunction.caller = nullptr;
    this->m_hasPendingTimer = false;
    this->m_hasStatementStart = false;
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_resume(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    FW_ASSERT(this->m_wasm != nullptr);
    auto status = spacewasm_resume(this->m_wasm);
    FW_ASSERT(status == SPACEWASM_OK, status);
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_resumeI32(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal,
    I32 value) {
    FW_ASSERT(this->m_wasm != nullptr);
    spacewasm_value_t return_val;
    return_val.tag = SPACEWASM_I32;
    return_val.u.i32_ = value;

    auto status = spacewasm_resume_value(this->m_wasm, return_val);
    FW_ASSERT(status == SPACEWASM_OK, status);
}

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_checkSleepTimers(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
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

void WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_action_checkTimeout(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) {
    // Only blocking async host functions that await an external reply are subject
    // to the statement timeout (COMMAND -> cmdResponseIn, ASYNC_PORT -> serialReply).
    // Sleeps have their own wake timer (checkShouldWake) and are not "statements".
    if (!this->m_hasStatementStart) {
        return;
    }

    Fw::ParamValid prmValid;
    const F32 timeoutSecs = this->paramGet_STATEMENT_TIMEOUT_SECS(prmValid);

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

    Fw::Time deadline = this->m_statementStart;
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
            // Time base / context changed since the statement started.
            this->interpreter_sendSignal_hostResponseTimeIncomparable();
            break;
    }
}

// ----------------------------------------------------------------------
// Implementations for internal state machine guards
// ----------------------------------------------------------------------

bool WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_guard_pendingPause(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) const {
    return this->m_pendingPause;
}

bool WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_guard_pendingHostFunction(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) const {
    return this->m_pendingHostFunction.isPending();
}

bool WasmSequencer ::Svc_WasmSequencer_EngineStateMachine_guard_pendingHostFunctionIsSleep(
    SmId smId,
    Svc_WasmSequencer_EngineStateMachine::Signal signal) const {
    return this->m_pendingHostFunction.kind == WasmSequencer_HostFunction::ASLEEP ||
           this->m_pendingHostFunction.kind == WasmSequencer_HostFunction::RSLEEP;
}

}  // namespace Svc
