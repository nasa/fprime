// ======================================================================
// \title  WasmSequencerHost.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer Wasm Host functions
// ======================================================================

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Log/LogSeverityEnumAc.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/ExternalString.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencer_HostFunctionEnumAc.hpp"
#include "config/FwPacketDescriptorTypeAliasAc.h"
#include "spacewasm.h"

namespace Svc {

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
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::wasmReadTelemetry(spacewasm_caller_t* caller,
                                                             const spacewasm_value_t* params,
                                                             size_t n_params,
                                                             spacewasm_value_t*) {
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::wasmReadParameter(spacewasm_caller_t* caller,
                                                             const spacewasm_value_t* params,
                                                             size_t n_params,
                                                             spacewasm_value_t*) {
    return SPACEWASM_TRAP;
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
        this->log_WARNING_HI_CommandTooLarge(len, FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType));
        return_status = SPACEWASM_TRAP;
    } else {
        this->m_pendingHostFunction.kind = PendingHostFunction::COMMAND;
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
    this->m_pendingHostFunction.kind = PendingHostFunction::EVENT;
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
    this->m_pendingHostFunction.kind = PendingHostFunction::RSLEEP;
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
    this->m_pendingHostFunction.kind = PendingHostFunction::ASLEEP;
    this->m_pendingHostFunction.caller = caller;
    this->m_pendingHostFunction.time_us = us;

    return SPACEWASM_PAUSE;
}

}  // namespace Svc
