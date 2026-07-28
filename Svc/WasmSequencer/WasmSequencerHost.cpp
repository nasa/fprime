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
// A helper template type that converts C++ member functions into pure
template <typename T>
struct CppCallback;

// Specialization for non-const member functions
template <typename Class, typename Ret, typename... Args>
struct CppCallback<Ret (Class::*)(Args...)> {
    template <Ret (Class::*MemberFn)(Args...)>
    static Ret c_fn(void* user_data, Args... args) noexcept {
        auto* self = static_cast<Class*>(user_data);
        try {
            return (self->*MemberFn)(std::forward<Args>(args)...);
        } catch (...) {
            return Ret();  // Default fallback on exception
        }
    }
};

#define C_CB(mem_fn) &CppCallback<decltype(mem_fn)>::c_fn<mem_fn>

spacewasm_hostcall_result_t WasmSequencer::wasmExit(spacewasm_caller_t*,
                                                    const spacewasm_value_t*,
                                                    size_t,
                                                    spacewasm_value_t*) {
    // TODO(tumbar) Add "exit" return code to SpaceWasm host functions
    //              This is basically the same as trap but not 'error-ey'
    // Exit the interpreter
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::wasmPanic(spacewasm_caller_t* caller,
                                                     const spacewasm_value_t* params,
                                                     size_t n_params,
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
    // These are automatically validated by spacewasm so it should be safe to assert them
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 2, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);

    const U32 ptr = static_cast<U32>(params[0].u.i32_);
    const U32 len = static_cast<U32>(params[1].u.i32_);

    spacewasm_hostcall_result_t return_status;
    if ((len + sizeof(FwPacketDescriptorType)) > FW_COM_BUFFER_MAX_SIZE) {
        this->log_WARNING_HI_CommandTooLarge(len, FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType));
        return_status = SPACEWASM_TRAP;
    } else {
        FW_ASSERT(!this->m_hasPendingHostFunction);

        // Write the CMD descriptor to the ComBuffer
        this->m_pendingHostFunction.buffer.resetSer();
        auto serStatus = this->m_pendingHostFunction.buffer.serializeFrom(
            static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND));
        FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

        // Read the command buffer into our ComBuffer
        const auto status = spacewasm_mem_read(
            caller, ptr, this->m_pendingHostFunction.buffer.getBuffAddr() + sizeof(FwPacketDescriptorType), len);
        if (status != SPACEWASM_OK) {
            // Memory read failed
            this->log_WARNING_HI_HostFunctionInvalidPointer(Svc::WasmSequencer_HostFunction::COMMAND,
                                                            static_cast<WasmSequencer_Status::T>(status));
            return_status = SPACEWASM_TRAP;
        } else {
            // Memory read succeeded. Pause the interpreter and process it in the state machine
            this->m_pendingHostFunction.kind = PendingHostInvocation::COMMAND;
            this->m_pendingHostFunction.buffer.moveSerToOffset(len + sizeof(FwPacketDescriptorType));
            this->m_hasPendingHostFunction = true;

            // Always pause the interpreter to allow the state machine to process this request
            return_status = SPACEWASM_PAUSE;
        }
    }

    return return_status;
}

spacewasm_hostcall_result_t WasmSequencer::wasmEvent(spacewasm_caller_t* caller,
                                                     const spacewasm_value_t* params,
                                                     size_t n_params,
                                                     spacewasm_value_t*) {
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

    U8 stringStorage[FW_LOG_STRING_MAX_SIZE];

    U32 len = static_cast<U32>(params[2].u.i32_);
    if (len > FW_LOG_STRING_MAX_SIZE) {
        len = FW_LOG_STRING_MAX_SIZE;
    }

    auto status = spacewasm_mem_read(caller, static_cast<U32>(params[1].u.i32_), stringStorage, len);
    if (status != SPACEWASM_OK) {
        this->log_WARNING_HI_HostFunctionInvalidPointer(
            Svc::WasmSequencer_HostFunction::EVENT,
            Svc::WasmSequencer_Status(static_cast<WasmSequencer_Status::T>(status)));
        return SPACEWASM_TRAP;
    }

    Fw::LogSeverity severity(static_cast<Fw::LogSeverity::T>(params[0].u.i32_));
    Fw::ExternalString msg(reinterpret_cast<char*>(stringStorage), len);

    switch (severity) {
        case Fw::LogSeverity::FATAL:
            this->log_FATAL_GuestFatal(msg);
            break;
        case Fw::LogSeverity::WARNING_HI:
            this->log_WARNING_HI_GuestWarningHi(msg);
            break;
        case Fw::LogSeverity::WARNING_LO:
            this->log_WARNING_LO_GuestWarningLo(msg);
            break;
        case Fw::LogSeverity::COMMAND:
            this->log_COMMAND_GuestCommand(msg);
            break;
        case Fw::LogSeverity::ACTIVITY_HI:
            this->log_ACTIVITY_HI_GuestActivityHi(msg);
            break;
        case Fw::LogSeverity::ACTIVITY_LO:
            this->log_ACTIVITY_LO_GuestActivityLo(msg);
            break;
        case Fw::LogSeverity::DIAGNOSTIC:
            this->log_DIAGNOSTIC_GuestDiagnostic(msg);
            break;
    }

    return SPACEWASM_CONTINUE_NONE;
}

spacewasm_hostcall_result_t WasmSequencer::wasmRsleep(spacewasm_caller_t* caller,
                                                      const spacewasm_value_t* params,
                                                      size_t n_params,
                                                      spacewasm_value_t*) {
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::wasmAsleep(spacewasm_caller_t* caller,
                                                      const spacewasm_value_t* params,
                                                      size_t n_params,
                                                      spacewasm_value_t*) {
    return SPACEWASM_TRAP;
}

}  // namespace Svc
