// ======================================================================
// \title  WasmSequencerHost.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer Wasm Host functions
// ======================================================================

#include <limits>

#include "Fw/Types/Assert.hpp"
#include "Svc/Seq/BlockStateEnumAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencerComponentAc.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "config/FppConstantsAc.hpp"
#include "config/FwChanIdTypeAliasAc.h"
#include "config/FwPacketDescriptorTypeAliasAc.h"
#include "config/FwPrmIdTypeAliasAc.h"
#include "spacewasm.h"

namespace Svc {

void WasmSequencer ::hostFprimeV1(spacewasm_host_t* host) {
#define HOST_FN(member_fn)                                                                                         \
    [](struct spacewasm_caller_t* caller, void* userdata, const struct spacewasm_value_t* params, size_t n_params, \
       struct spacewasm_value_t* out_result) {                                                                     \
        FW_ASSERT(userdata != nullptr);                                                                            \
        return static_cast<WasmSequencer*>(userdata)->member_fn(caller, params, n_params, out_result);             \
    }

    spacewasm_status_t status;
    U32 module_idx;
    status = spacewasm_add_host_module(host, "fprime_v1", 12, 0, &module_idx);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "exit", "i", "", HOST_FN(wasmExit), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "panic", "i", "", HOST_FN(wasmPanic), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "args", "ii", "i", HOST_FN(wasmArgs), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "time", "ii", "", HOST_FN(wasmTime), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "tlm", "Iiiii", "i", HOST_FN(wasmReadTelemetry), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "prm", "Iii", "i", HOST_FN(wasmReadParameter), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "cmd", "ii", "i", HOST_FN(wasmCommand), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "event", "iii", "", HOST_FN(wasmEvent), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "rsleep", "I", "", HOST_FN(wasmRsleep), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "asleep", "I", "", HOST_FN(wasmAsleep), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "serial_send", "iii", "", HOST_FN(wasmSerialOut), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "serial_recv", "iiiii", "i", HOST_FN(wasmSerialRecv), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

#undef HOST_FN
}

spacewasm_hostcall_result_t WasmSequencer::wasmExit(spacewasm_caller_t* caller,
                                                    const spacewasm_value_t* params,
                                                    size_t n_params,
                                                    spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 1, static_cast<FwAssertArgType>(n_params));

    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);

    this->m_exit.reason = WasmSequencer_ExitReason::HOST_EXIT;
    this->m_exit.code = params[0].u.i32_;

    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::wasmPanic(spacewasm_caller_t* caller,
                                                     const spacewasm_value_t* params,
                                                     size_t n_params,
                                                     spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 1, static_cast<FwAssertArgType>(n_params));

    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);

    this->m_exit.reason = WasmSequencer_ExitReason::HOST_PANIC;
    this->m_exit.code = params[0].u.i32_;

    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::wasmArgs(spacewasm_caller_t* caller,
                                                    const spacewasm_value_t* params,
                                                    size_t n_params,
                                                    spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 2, static_cast<FwAssertArgType>(n_params));

    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);

    const U32 ptr = static_cast<U32>(params[0].u.i32_);
    const U32 size = static_cast<U32>(params[1].u.i32_);

    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::ARGS;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.u.args.ptr = ptr;
    this->m_pendingHostFunction.u.args.len = size;

    return SPACEWASM_PAUSE;
}

spacewasm_hostcall_result_t WasmSequencer::wasmTime(spacewasm_caller_t* caller,
                                                    const spacewasm_value_t* params,
                                                    size_t n_params,
                                                    spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 2, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);

    const U32 time_ptr = static_cast<U32>(params[0].u.i32_);
    const U32 time_size = static_cast<U32>(params[1].u.i32_);

    spacewasm_hostcall_result_t return_status;
    if (time_size < Fw::Time::SERIALIZED_SIZE) {
        // We expect an exact match for serialized time
        this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::TIME, time_size, Fw::Time::SERIALIZED_SIZE);
        return_status = SPACEWASM_TRAP;
    } else if (time_size > Fw::Time::SERIALIZED_SIZE) {
        // We expect an exact match for serialized time
        this->log_WARNING_HI_BufferTooLarge(WasmSequencer_HostFunction::TIME, time_size, Fw::Time::SERIALIZED_SIZE);
        return_status = SPACEWASM_TRAP;
    } else {
        this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::TIME;
        this->m_pendingHostFunction.caller = caller;
        this->m_pendingHostFunction.u.time.ptr = time_ptr;
        this->m_pendingHostFunction.u.time.len = time_size;

        // Always pause the interpreter to allow the state machine to process this request
        return_status = SPACEWASM_PAUSE;
    }

    return return_status;
}

spacewasm_hostcall_result_t WasmSequencer::wasmReadTelemetry(spacewasm_caller_t* caller,
                                                             const spacewasm_value_t* params,
                                                             size_t n_params,
                                                             spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    // These are automatically validated by spacewasm so it should be safe to assert them
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 5, static_cast<FwAssertArgType>(n_params));

    // FwChanIdType always in 64-bits
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I64, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);
    FW_ASSERT(params[2].tag == spacewasm_valtype_t::SPACEWASM_I32, params[2].tag);
    FW_ASSERT(params[3].tag == spacewasm_valtype_t::SPACEWASM_I32, params[3].tag);
    FW_ASSERT(params[4].tag == spacewasm_valtype_t::SPACEWASM_I32, params[4].tag);

    I64 id = params[0].u.i64_;
    const U32 time_ptr = static_cast<U32>(params[1].u.i32_);
    const U32 time_size = static_cast<U32>(params[2].u.i32_);
    const U32 value_ptr = static_cast<U32>(params[3].u.i32_);
    const U32 value_size = static_cast<U32>(params[4].u.i32_);

    // The telemetry port is a plain (not required) output port, so guard against an
    // unconnected deployment before dispatching; the generated invoker would FW_ASSERT.
    if (!this->isConnected_getTlmChan_OutputPort(0)) {
        this->log_WARNING_HI_HostFunctionInvalidPort(WasmSequencer_HostFunction::TELEMETRY, 0, 0);
        return SPACEWASM_TRAP;
    }

    spacewasm_hostcall_result_t return_status;
    if (time_size < Fw::Time::SERIALIZED_SIZE) {
        // We expect an exact match for serialized time
        this->log_WARNING_HI_BufferTooSmall(WasmSequencer_HostFunction::TELEMETRY, time_size,
                                            Fw::Time::SERIALIZED_SIZE);
        return_status = SPACEWASM_TRAP;
    } else if (time_size > Fw::Time::SERIALIZED_SIZE) {
        // We expect an exact match for serialized time
        this->log_WARNING_HI_BufferTooLarge(WasmSequencer_HostFunction::TELEMETRY, time_size,
                                            Fw::Time::SERIALIZED_SIZE);
        return_status = SPACEWASM_TRAP;
    } else {
        this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::TELEMETRY;
        this->m_pendingHostFunction.caller = caller;
        this->m_pendingHostFunction.u.telemetry.chanId = static_cast<FwChanIdType>(id);
        this->m_pendingHostFunction.u.telemetry.timePtr = time_ptr;
        this->m_pendingHostFunction.u.telemetry.timeLen = time_size;
        this->m_pendingHostFunction.u.telemetry.valuePtr = value_ptr;
        this->m_pendingHostFunction.u.telemetry.valueLen = value_size;

        // Always pause the interpreter to allow the state machine to process this request
        return_status = SPACEWASM_PAUSE;
    }

    return return_status;
}

spacewasm_hostcall_result_t WasmSequencer::wasmReadParameter(spacewasm_caller_t* caller,
                                                             const spacewasm_value_t* params,
                                                             size_t n_params,
                                                             spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    // These are automatically validated by spacewasm so it should be safe to assert them
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 3, static_cast<FwAssertArgType>(n_params));

    // FwPrmIdType always in 64-bits
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I64, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);
    FW_ASSERT(params[2].tag == spacewasm_valtype_t::SPACEWASM_I32, params[2].tag);

    I64 id = params[0].u.i64_;
    const U32 ptr = static_cast<U32>(params[1].u.i32_);
    const U32 len = static_cast<U32>(params[2].u.i32_);

    // The parameter port is a plain (not required) output port, so guard against an
    // unconnected deployment before dispatching; the generated invoker would FW_ASSERT.
    if (!this->isConnected_getParam_OutputPort(0)) {
        this->log_WARNING_HI_HostFunctionInvalidPort(WasmSequencer_HostFunction::PARAMETER, 0, 0);
        return SPACEWASM_TRAP;
    }

    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::PARAMETER;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.u.parameter.prmId = static_cast<FwPrmIdType>(id);
    this->m_pendingHostFunction.u.parameter.ptr = ptr;
    this->m_pendingHostFunction.u.parameter.len = len;

    // Always pause the interpreter to allow the state machine to process this request
    return SPACEWASM_PAUSE;
}

spacewasm_hostcall_result_t WasmSequencer::wasmCommand(struct spacewasm_caller_t* caller,
                                                       const struct spacewasm_value_t* params,
                                                       size_t n_params,
                                                       struct spacewasm_value_t* out_result) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    // These are automatically validated by spacewasm so it should be safe to assert them
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 2, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);

    const U32 ptr = static_cast<U32>(params[0].u.i32_);
    const U32 len = static_cast<U32>(params[1].u.i32_);

    // cmdOut is a plain (not required) output port, so guard against an unconnected
    // deployment before dispatching; the generated invoker would FW_ASSERT.
    if (!this->isConnected_cmdOut_OutputPort(0)) {
        this->log_WARNING_HI_HostFunctionInvalidPort(WasmSequencer_HostFunction::COMMAND, 0, 0);
        return SPACEWASM_TRAP;
    }

    constexpr const U32 maxPayload = FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType);
    spacewasm_hostcall_result_t return_status;
    if (len > maxPayload) {
        this->log_WARNING_HI_BufferTooLarge(WasmSequencer_HostFunction::COMMAND, len, maxPayload);
        return_status = SPACEWASM_TRAP;
    } else {
        this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::COMMAND;
        this->m_pendingHostFunction.caller = caller;
        this->m_pendingHostFunction.u.command.ptr = ptr;
        this->m_pendingHostFunction.u.command.len = len;

        // Always pause the interpreter to allow the state machine to process this request
        return_status = SPACEWASM_PAUSE;
    }

    return return_status;
}

spacewasm_hostcall_result_t WasmSequencer::wasmEvent(spacewasm_caller_t* caller,
                                                     const spacewasm_value_t* params,
                                                     size_t n_params,
                                                     spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    // These are automatically validated by spacewasm so it should be safe to assert them
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 3, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);
    FW_ASSERT(params[2].tag == spacewasm_valtype_t::SPACEWASM_I32, params[2].tag);

    // The requested severity is validated when the event is dispatched (see the
    // EVENT case in dispatchPendingHostFunction). A forbidden or out-of-range
    // severity is reported (with the guest message) rather than trapping, so we
    // read the message here in all cases and stash the raw id.
    U32 ptr = static_cast<U32>(params[1].u.i32_);
    U32 len = static_cast<U32>(params[2].u.i32_);
    if (len > FW_LOG_STRING_MAX_SIZE) {
        len = FW_LOG_STRING_MAX_SIZE;
    }

    // Pend this event dispatch to be executed by the sequencer state machine
    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::EVENT;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.u.event.rawSeverity = static_cast<U32>(params[0].u.i32_);
    this->m_pendingHostFunction.u.event.msgPtr = ptr;
    this->m_pendingHostFunction.u.event.msgLen = len;

    return SPACEWASM_PAUSE;
}

spacewasm_hostcall_result_t WasmSequencer::wasmRsleep(spacewasm_caller_t* caller,
                                                      const spacewasm_value_t* params,
                                                      size_t n_params,
                                                      spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 1, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I64, params[0].tag);

    const U64 us = static_cast<U64>(params[0].u.i64_);

    // The seconds part of the duration must fit the U32 fields of Fw::Time; a
    // guest value large enough to overflow it would silently truncate. Reject it.
    if ((us / 1000000u) > static_cast<U64>(std::numeric_limits<U32>::max())) {
        this->log_WARNING_HI_SleepDurationTooLarge(WasmSequencer_HostFunction::RSLEEP, us);
        return SPACEWASM_TRAP;
    }

    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::RSLEEP;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.u.sleep.us = us;

    return SPACEWASM_PAUSE;
}

spacewasm_hostcall_result_t WasmSequencer::wasmAsleep(spacewasm_caller_t* caller,
                                                      const spacewasm_value_t* params,
                                                      size_t n_params,
                                                      spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 1, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I64, params[0].tag);

    const U64 us = static_cast<U64>(params[0].u.i64_);

    // The seconds part of the duration must fit the U32 fields of Fw::Time; a
    // guest value large enough to overflow it would silently truncate. Reject it.
    if ((us / 1000000u) > static_cast<U64>(std::numeric_limits<U32>::max())) {
        this->log_WARNING_HI_SleepDurationTooLarge(WasmSequencer_HostFunction::ASLEEP, us);
        return SPACEWASM_TRAP;
    }

    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::ASLEEP;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.u.sleep.us = us;

    return SPACEWASM_PAUSE;
}

spacewasm_hostcall_result_t WasmSequencer::wasmSerialOut(spacewasm_caller_t* caller,
                                                         const spacewasm_value_t* params,
                                                         size_t n_params,
                                                         spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    // These are automatically validated by spacewasm so it should be safe to assert them
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 3, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);
    FW_ASSERT(params[2].tag == spacewasm_valtype_t::SPACEWASM_I32, params[2].tag);

    const I32 index = params[0].u.i32_;
    const U32 ptr = static_cast<U32>(params[1].u.i32_);
    const U32 len = static_cast<U32>(params[2].u.i32_);

    if (len > Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE) {
        this->log_WARNING_HI_BufferTooLarge(WasmSequencer_HostFunction::SERIAL_OUT, len,
                                            Svc::WasmSequencerConfig::MAX_SERIAL_PORT_SIZE);
        return SPACEWASM_TRAP;
    }

    if (index < 0 || index >= this->getNum_serialOut_OutputPorts() ||
        !this->isConnected_serialOut_OutputPort(static_cast<FwIndexType>(index))) {
        this->log_WARNING_HI_HostFunctionInvalidPort(WasmSequencer_HostFunction::SERIAL_OUT, index,
                                                     static_cast<U32>(this->getNum_serialOut_OutputPorts()));
        return SPACEWASM_TRAP;
    }

    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::SERIAL_OUT;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.u.serialOut.index = static_cast<U32>(index);
    this->m_pendingHostFunction.u.serialOut.ptr = ptr;
    this->m_pendingHostFunction.u.serialOut.len = len;

    // Always pause the interpreter to allow the state machine to process this request
    return SPACEWASM_PAUSE;
}

spacewasm_hostcall_result_t WasmSequencer::wasmSerialRecv(spacewasm_caller_t* caller,
                                                          const spacewasm_value_t* params,
                                                          size_t n_params,
                                                          spacewasm_value_t*) {
    FW_ASSERT(!this->m_pendingHostFunction.isPending());
    // These are automatically validated by spacewasm so it should be safe to assert them
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 5, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);
    FW_ASSERT(params[2].tag == spacewasm_valtype_t::SPACEWASM_I32, params[2].tag);
    FW_ASSERT(params[3].tag == spacewasm_valtype_t::SPACEWASM_I32, params[3].tag);
    FW_ASSERT(params[4].tag == spacewasm_valtype_t::SPACEWASM_I32, params[4].tag);

    const I32 index = params[0].u.i32_;
    const U32 data_ptr = static_cast<U32>(params[1].u.i32_);
    const U32 data_len = static_cast<U32>(params[2].u.i32_);
    const U32 actual_size_ptr = static_cast<U32>(params[3].u.i32_);
    const I32 block_type = params[4].u.i32_;

    if (index < 0 || index >= NUM_SERIALIN_INPUT_PORTS) {
        this->log_WARNING_HI_HostFunctionInvalidPort(WasmSequencer_HostFunction::SERIAL_RECV, index,
                                                     NUM_SERIALIN_INPUT_PORTS);
        return SPACEWASM_TRAP;
    }

    if (block_type < 0 || block_type > static_cast<I32>(std::numeric_limits<Svc::BlockState::SerialType>::max()) ||
        !Svc::BlockState::isValid(static_cast<Svc::BlockState::SerialType>(block_type))) {
        this->log_WARNING_HI_InvalidBlockingTypeValue(block_type);
        return SPACEWASM_TRAP;
    }

    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::SERIAL_RECV;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.u.serialRecv.index = static_cast<U32>(index);
    this->m_pendingHostFunction.u.serialRecv.dataPtr = data_ptr;
    this->m_pendingHostFunction.u.serialRecv.dataSize = data_len;
    this->m_pendingHostFunction.u.serialRecv.actualSizePtr = actual_size_ptr;
    this->m_pendingHostFunction.u.serialRecv.blockingType = static_cast<Svc::BlockState::T>(block_type);

    // Always pause the interpreter to allow the state machine to process this request
    return SPACEWASM_PAUSE;
}

Os::Mutex* WasmSequencer::getGlobalAllocatorLock() {
    static Os::Mutex globalAllocatorLock;
    return &globalAllocatorLock;
}

}  // namespace Svc
