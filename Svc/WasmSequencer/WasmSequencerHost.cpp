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
spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_exit(spacewasm_caller_t*,
                                                            void*,
                                                            const spacewasm_value_t*,
                                                            size_t,
                                                            spacewasm_value_t*) {
    // TODO(tumbar) Add "exit" return code to SpaceWasm host functions
    //              This is basically the same as trap but not 'error-ey'
    // Exit the interpreter
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_panic(spacewasm_caller_t* caller,
                                                             void* userdata,
                                                             const spacewasm_value_t* params,
                                                             size_t n_params,
                                                             spacewasm_value_t*) {
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_read_telemetry(spacewasm_caller_t* caller,
                                                                      void* userdata,
                                                                      const spacewasm_value_t* params,
                                                                      size_t n_params,
                                                                      spacewasm_value_t*) {
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_read_parameter(spacewasm_caller_t* caller,
                                                                      void* userdata,
                                                                      const spacewasm_value_t* params,
                                                                      size_t n_params,
                                                                      spacewasm_value_t*) {
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_command(spacewasm_caller_t* caller,
                                                               void* userdata,
                                                               const spacewasm_value_t* params,
                                                               size_t n_params,
                                                               spacewasm_value_t*) {
    auto* this_ = static_cast<WasmSequencer*>(userdata);

    // These are automatically validated by spacewasm so it should be safe to assert them
    FW_ASSERT(params != nullptr);
    FW_ASSERT(n_params == 2, static_cast<FwAssertArgType>(n_params));
    FW_ASSERT(params[0].tag == spacewasm_valtype_t::SPACEWASM_I32, params[0].tag);
    FW_ASSERT(params[1].tag == spacewasm_valtype_t::SPACEWASM_I32, params[1].tag);

    const U32 ptr = static_cast<U32>(params[0].u.i32_);
    const U32 len = static_cast<U32>(params[1].u.i32_);

    spacewasm_hostcall_result_t return_status;
    if ((len + sizeof(FwPacketDescriptorType)) > FW_COM_BUFFER_MAX_SIZE) {
        this_->log_WARNING_HI_CommandTooLarge(len, FW_COM_BUFFER_MAX_SIZE - sizeof(FwPacketDescriptorType));
        return_status = SPACEWASM_TRAP;
    } else {
        FW_ASSERT(!this_->m_hasPendingHostFunction);

        // Write the CMD descriptor to the ComBuffer
        this_->m_pendingHostFunction.buffer.resetSer();
        auto serStatus = this_->m_pendingHostFunction.buffer.serializeFrom(
            static_cast<FwPacketDescriptorType>(Fw::ComPacketType::FW_PACKET_COMMAND));
        FW_ASSERT(serStatus == Fw::FW_SERIALIZE_OK, serStatus);

        // Read the command buffer into our ComBuffer
        const auto status = spacewasm_mem_read(
            caller, ptr, this_->m_pendingHostFunction.buffer.getBuffAddr() + sizeof(FwPacketDescriptorType), len);
        if (status != SPACEWASM_OK) {
            // Memory read failed
            this_->log_WARNING_HI_CommandInvalidPointer(static_cast<WasmSequencer_SpaceWasmStatus::T>(status));
            return_status = SPACEWASM_TRAP;
        } else {
            // Memory read succeeded. Pause the interpreter and process it in the state machine
            this_->m_pendingHostFunction.kind = PendingHostInvocation::COMMAND;
            this_->m_pendingHostFunction.buffer.moveSerToOffset(len + sizeof(FwPacketDescriptorType));
            this_->m_hasPendingHostFunction = true;

            // Always pause the interpreter to allow the state machine to process this request
            return_status = SPACEWASM_PAUSE;
        }
    }

    return return_status;
}

spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_event(spacewasm_caller_t* caller,
                                                             void* userdata,
                                                             const spacewasm_value_t* params,
                                                             size_t n_params,
                                                             spacewasm_value_t*) {
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_rsleep(spacewasm_caller_t* caller,
                                                              void* userdata,
                                                              const spacewasm_value_t* params,
                                                              size_t n_params,
                                                              spacewasm_value_t*) {
    return SPACEWASM_TRAP;
}

spacewasm_hostcall_result_t WasmSequencer::fprime_wasm_asleep(spacewasm_caller_t* caller,
                                                              void* userdata,
                                                              const spacewasm_value_t* params,
                                                              size_t n_params,
                                                              spacewasm_value_t*) {
    return SPACEWASM_TRAP;
}

}  // namespace Svc
