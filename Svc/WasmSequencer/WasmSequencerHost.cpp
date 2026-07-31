// ======================================================================
// \title  WasmSequencerHost.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer Wasm Host functions
// ======================================================================

#include "Fw/Log/LogSeverityEnumAc.hpp"
#include "Fw/Types/Assert.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "config/FppConstantsAc.hpp"
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
    status = spacewasm_add_host_module(host, "fprime_v1", 8, 0, &module_idx);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "exit", "i", "", HOST_FN(wasmExit), this);
    FW_ASSERT(status == SPACEWASM_OK, status);

    status = spacewasm_add_host_function(host, module_idx, "panic", "i", "", HOST_FN(wasmPanic), this);
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

#undef HOST_FN
}

spacewasm_hostcall_result_t WasmSequencer::wasmExit(spacewasm_caller_t*,
                                                    const spacewasm_value_t*,
                                                    size_t,
                                                    spacewasm_value_t*) {
    // TODO(tumbar) Add "exit" return code to SpaceWasm host functions
    //              This is basically the same as trap but not 'error-ey'
    // Exit the interpreter
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::wasmPanic(spacewasm_caller_t*,
                                                     const spacewasm_value_t*,
                                                     size_t,
                                                     spacewasm_value_t*) {
    // TODO(tumbar) Extract status code
    return SPACEWASM_TRAP;
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

    spacewasm_hostcall_result_t return_status;
    if (value_size > FW_TLM_BUFFER_MAX_SIZE) {
        this->log_WARNING_HI_BufferTooLarge(WasmSequencer_HostFunction::TELEMETRY, value_size, FW_TLM_BUFFER_MAX_SIZE);
        return_status = SPACEWASM_TRAP;
    } else if (time_size != Fw::Time::SERIALIZED_SIZE) {
        // We expect an expact match for serialized time
        this->log_WARNING_HI_BufferTooLarge(WasmSequencer_HostFunction::TELEMETRY, time_size,
                                            Fw::Time::SERIALIZED_SIZE);
        return_status = SPACEWASM_TRAP;
    } else {
        this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::TELEMETRY;
        this->m_pendingHostFunction.caller = caller;
        this->m_pendingHostFunction.prm_id = static_cast<FwPrmIdType>(id);
        this->m_pendingHostFunction.ptr1 = time_ptr;
        this->m_pendingHostFunction.len1 = time_size;
        this->m_pendingHostFunction.ptr2 = value_ptr;
        this->m_pendingHostFunction.len2 = value_size;

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

    spacewasm_hostcall_result_t return_status;
    if (len > FW_PARAM_BUFFER_MAX_SIZE) {
        this->log_WARNING_HI_BufferTooLarge(WasmSequencer_HostFunction::PARAMETER, len, FW_PARAM_BUFFER_MAX_SIZE);
        return_status = SPACEWASM_TRAP;
    } else {
        this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::PARAMETER;
        this->m_pendingHostFunction.caller = caller;
        this->m_pendingHostFunction.prm_id = static_cast<FwPrmIdType>(id);
        this->m_pendingHostFunction.ptr1 = ptr;
        this->m_pendingHostFunction.len1 = len;

        // Always pause the interpreter to allow the state machine to process this request
        return_status = SPACEWASM_PAUSE;
    }

    return return_status;
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

    spacewasm_hostcall_result_t return_status;
    if (len + sizeof(FwPacketDescriptorType) > FW_COM_BUFFER_MAX_SIZE) {
        this->log_WARNING_HI_BufferTooLarge(WasmSequencer_HostFunction::COMMAND, len,
                                            FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType));
        return_status = SPACEWASM_TRAP;
    } else {
        this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::COMMAND;
        this->m_pendingHostFunction.caller = caller;
        this->m_pendingHostFunction.ptr1 = ptr;
        this->m_pendingHostFunction.len1 = len;

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

    if (!Fw::LogSeverity::isValid(static_cast<Fw::LogSeverity::SerialType>(params[0].u.i32_))) {
        this->log_WARNING_HI_HostFunctionInvalidSeverity(params[0].u.i32_);
        return SPACEWASM_TRAP;
    }

    U32 ptr = static_cast<U32>(params[1].u.i32_);
    U32 len = static_cast<U32>(params[2].u.i32_);
    if (len > FW_LOG_STRING_MAX_SIZE) {
        len = FW_LOG_STRING_MAX_SIZE;
    }

    // Pend this event dispatch to be executed by the sequencer state machine
    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::EVENT;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.severity = static_cast<Fw::LogSeverity::T>(params[0].u.i32_);
    this->m_pendingHostFunction.ptr1 = ptr;
    this->m_pendingHostFunction.len1 = len;

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
    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::RSLEEP;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.time_us = us;

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
    this->m_pendingHostFunction.kind = WasmSequencer_HostFunction::ASLEEP;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.time_us = us;

    return SPACEWASM_PAUSE;
}

Os::Mutex* WasmSequencer::getGlobalAllocatorLock() {
    static Os::Mutex globalAllocatorLock;
    return &globalAllocatorLock;
}

}  // namespace Svc
