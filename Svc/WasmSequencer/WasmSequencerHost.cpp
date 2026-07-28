// ======================================================================
// \title  WasmSequencerHost.cpp
// \author tumbar
// \brief  cpp file for WasmSequencer Wasm Host functions
// ======================================================================

#include "Fw/Com/ComPacket.hpp"
#include "Fw/Types/Assert.hpp"
#include "Fw/Types/Serializable.hpp"
#include "Svc/WasmSequencer/WasmSequencer.hpp"
#include "Svc/WasmSequencer/WasmSequencer_SpaceWasmStatusEnumAc.hpp"
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
            this->log_WARNING_HI_CommandInvalidPointer(static_cast<WasmSequencer_SpaceWasmStatus::T>(status));
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
    return SPACEWASM_TRAP;
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
